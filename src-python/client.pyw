"""
client.pyw — Client Gestione Presenze (per i PC dipendenti)
Si collega al server e apre una finestra nativa.

PRIMA DI COMPILARE: modifica SERVER_IP con l'IP del tuo server!
"""

import webview
import sys

# ╔══════════════════════════════════════════════════╗
# ║  MODIFICA QUI L'IP DEL TUO SERVER               ║
# ║  Esempio: "192.168.1.100"                        ║
# ╚══════════════════════════════════════════════════╝
SERVER_IP = "192.168.1.100"
SERVER_PORT = 5000


def main():
    url = f"http://{SERVER_IP}:{SERVER_PORT}"
    window = webview.create_window(
        "Gestione Presenze",
        url,
        width=1100,
        height=750,
        min_size=(480, 600),
        resizable=True,
        text_select=False,
    )
    webview.start()


if __name__ == "__main__":
    main()
