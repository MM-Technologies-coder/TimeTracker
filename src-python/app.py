"""
app.py — Server Gestione Presenze
Questo file gira sul SERVER (o sul tuo PC admin).
Avvio:  python app.py
"""

from flask import Flask, request, jsonify, send_from_directory
import sqlite3
from datetime import datetime
import os
import sys

app = Flask(__name__)

# Il database si crea nella stessa cartella dell'exe/script
if getattr(sys, 'frozen', False):
    BASE_DIR = os.path.dirname(sys.executable)
else:
    BASE_DIR = os.path.dirname(os.path.abspath(__file__))

DB_PATH = os.path.join(BASE_DIR, "dati_aziendali.db")


def get_db():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    conn.execute("PRAGMA foreign_keys = ON")
    return conn


def init_db():
    conn = get_db()
    conn.execute("""
        CREATE TABLE IF NOT EXISTS utenti (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            ruolo INTEGER NOT NULL
        )
    """)
    conn.execute("""
        CREATE TABLE IF NOT EXISTS log_lavori (
            id_log INTEGER PRIMARY KEY AUTOINCREMENT,
            id_utente INTEGER NOT NULL,
            data TEXT NOT NULL,
            orario_login TEXT NOT NULL,
            orario_logout TEXT,
            minuti_pausa INTEGER DEFAULT 0,
            report TEXT,
            FOREIGN KEY (id_utente) REFERENCES utenti(id)
        )
    """)
    try:
        conn.execute("INSERT INTO utenti (username, ruolo) VALUES (?, ?)", ("admin", 1))
    except sqlite3.IntegrityError:
        pass
    conn.commit()
    conn.close()
    print(f"[OK] Database pronto: {DB_PATH}")


# ─── Serve i file statici ───
@app.route("/")
def index():
    return send_from_directory(BASE_DIR, "index.html")


# ─── API Login ───
@app.route("/api/login", methods=["POST"])
def api_login():
    data = request.json
    username = data.get("username", "").strip()
    if not username:
        return jsonify({"ok": False, "error": "Username vuoto"}), 400

    conn = get_db()
    user = conn.execute("SELECT id, ruolo FROM utenti WHERE username = ?", (username,)).fetchone()
    if not user:
        conn.close()
        return jsonify({"ok": False, "error": "Utente non trovato"}), 404

    ruolo = user["ruolo"]

    if ruolo == 0:
        # Controlla se ha gia' un login aperto oggi
        oggi = datetime.now().strftime("%Y-%m-%d")
        aperto = conn.execute(
            "SELECT id_log FROM log_lavori WHERE id_utente = ? AND data = ? AND orario_logout = 'In corso...'",
            (user["id"], oggi)
        ).fetchone()

        if not aperto:
            now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            conn.execute(
                "INSERT INTO log_lavori (id_utente, data, orario_login, orario_logout) VALUES (?, ?, ?, ?)",
                (user["id"], oggi, now, "In corso...")
            )
            conn.commit()

    conn.close()
    return jsonify({"ok": True, "ruolo": ruolo, "username": username})


# ─── API Pausa ───
@app.route("/api/pause", methods=["POST"])
def api_pause():
    data = request.json
    username = data.get("username")
    minuti = data.get("minuti", 0)

    conn = get_db()
    user = conn.execute("SELECT id FROM utenti WHERE username = ?", (username,)).fetchone()
    if not user:
        conn.close()
        return jsonify({"ok": False, "error": "Utente non trovato"}), 404

    conn.execute(
        "UPDATE log_lavori SET minuti_pausa = minuti_pausa + ? WHERE id_utente = ? AND orario_logout = 'In corso...'",
        (minuti, user["id"])
    )
    conn.commit()
    conn.close()
    return jsonify({"ok": True})


# ─── API Logout ───
@app.route("/api/logout", methods=["POST"])
def api_logout():
    data = request.json
    username = data.get("username")
    report = data.get("report", "")

    conn = get_db()
    user = conn.execute("SELECT id FROM utenti WHERE username = ?", (username,)).fetchone()
    if not user:
        conn.close()
        return jsonify({"ok": False, "error": "Utente non trovato"}), 404

    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    conn.execute(
        "UPDATE log_lavori SET orario_logout = ?, report = ? WHERE id_utente = ? AND orario_logout = 'In corso...'",
        (now, report, user["id"])
    )
    conn.commit()
    conn.close()
    return jsonify({"ok": True})


# ─── API Admin: log presenze ───
@app.route("/api/admin/logs")
def api_admin_logs():
    conn = get_db()
    rows = conn.execute("""
        SELECT u.username, l.data, l.orario_login, l.orario_logout, l.minuti_pausa,
               CASE
                 WHEN l.orario_logout = 'In corso...' THEN NULL
                 ELSE ROUND(((strftime('%s', l.orario_logout) - strftime('%s', l.orario_login)) / 60.0) - l.minuti_pausa, 2)
               END AS minuti_reali,
               l.report
        FROM log_lavori l
        JOIN utenti u ON l.id_utente = u.id
        ORDER BY l.data DESC, l.orario_login DESC
    """).fetchall()

    result = []
    for r in rows:
        result.append({
            "username": r["username"],
            "data": r["data"],
            "login": r["orario_login"],
            "logout": r["orario_logout"] or "In corso...",
            "pausa": r["minuti_pausa"],
            "minuti_reali": r["minuti_reali"],
            "report": r["report"] or ""
        })
    conn.close()
    return jsonify(result)


# ─── API Admin: aggiungi dipendente ───
@app.route("/api/admin/add-user", methods=["POST"])
def api_add_user():
    data = request.json
    username = data.get("username", "").strip()
    if not username:
        return jsonify({"ok": False, "error": "Username vuoto"}), 400
    conn = get_db()
    try:
        conn.execute("INSERT INTO utenti (username, ruolo) VALUES (?, 0)", (username,))
        conn.commit()
        conn.close()
        return jsonify({"ok": True})
    except sqlite3.IntegrityError:
        conn.close()
        return jsonify({"ok": False, "error": "Username gia' esistente"}), 409


# ─── API Admin: lista utenti ───
@app.route("/api/admin/users")
def api_admin_users():
    conn = get_db()
    rows = conn.execute("SELECT username, ruolo FROM utenti ORDER BY ruolo DESC, username").fetchall()
    conn.close()
    return jsonify([{"username": r["username"], "ruolo": r["ruolo"]} for r in rows])


if __name__ == "__main__":
    init_db()
    print("=" * 50)
    print("  GESTIONE PRESENZE — SERVER AVVIATO")
    print("  Apri: http://localhost:5000")
    print("  I dipendenti si collegano al tuo IP sulla porta 5000")
    print("=" * 50)
    # host='0.0.0.0' rende il server accessibile da tutta la rete
    app.run(host="0.0.0.0", port=5000, debug=False)
