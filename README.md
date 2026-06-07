# Time Tracker - Enterprise Attendance Management System 🏢⏱️

**Time Tracker** è un sistema stand-alone e client-server progettato per il monitoraggio e la gestione delle presenze aziendali in tempo reale. Il progetto nasce dall'esigenza di offrire una soluzione snella, sicura e a "manutenzione zero" per le imprese, eliminando la dipendenza da pesanti infrastrutture cloud esterne.

Il progetto mette in luce un doppio approccio ingegneristico: un'architettura di rete distribuita in **Python/Flask** per la produzione e un modulo core altamente performante sviluppato in **C++** per la logica dei dati.

---

## 🏗️ Architettura del Software & Tech Stack

Il sistema è suddiviso in due componenti principali e supporta una gestione dei ruoli (Admin/Dipendente):

### 1. Produzione: Soluzione Client-Server (Python & Web)
* **Backend Server:** Sviluppato in **Python** con il micro-framework **Flask**, che espone un'API REST per la gestione di login, logout, pause e pannello amministrativo.
* **Database Relazionale:** Integrazione con **SQLite 3** utilizzando query ottimizzate e vincoli di integrità referenziale (`FOREIGN KEY = ON`).
* **Frontend UI:** Interfaccia utente reattiva in **HTML5, CSS3 (Custom Variables, Dark Theme)** e **JavaScript (Async/Await Fetch API)**.
* **Desktop Client:** Applicazione client nativa per i dipendenti basata su **PyWebView**, ottimizzata per connettersi direttamente all'IP del server aziendale in rete locale.

### 2. Core Engine: Modulo Nativo (C++)
* Fornito come modulo standalone (`Database.cpp`), implementa la logica di business a basso livello e l'interfaccia diretta con l'API C/C++ di `sqlite3`, garantendo massime prestazioni nel calcolo dei tempi reali tramite funzioni temporali nativizzate.

---

## 🛠️ Funzionalità Chiave

* **Autenticazione a due livelli:** Accesso differenziato per Amministratori (Pannello di controllo, storico dei log, aggiunta dipendenti) e Dipendenti.
* **Real-time Tracking:** Calcolo automatico in tempo reale dei minuti effettivi di lavoro, scorporando i tempi di pausa.
* **Report di fine giornata:** Possibilità per il dipendente di inserire un sommario delle attività svolte prima del logout.
* **Automated Building:** Script batch (`build.bat`) integrato per la compilazione automatica degli esecutivi staccati tramite **PyInstaller**.

---

## 🚀 Come Avviare e Compilare il Progetto

### Prerequisiti
* Python 3.12 o superiore
* Compilatore G++ / C++ (per il modulo `Database.cpp`)

### 1. Avvio rapido in ambiente di sviluppo (Python)
Posizionati nella cartella del server ed esegui:
```bash
pip install flask
python app.py
```

**Il server sarà attivo su http://localhost:5000**

---

## 📈 Competenze Dimostrate in questo Progetto
Questo progetto riflette solide basi di ingegneria del software e networking, in particolare:

Database Design: Progettazione di schemi relazionali corretti, gestione di indici UNIQUE e query aggregate complesse (strftime, ROUND).

Network & Security: Logica client-server basata su protocollo HTTP, gestione di endpoint API REST e configurazione di socket su IP statici locali (es. ambiente Cisco/Zyxel).

Problem Solving & Ottimizzazione: Scrittura di codice asincrono nel frontend e gestione della memoria/prestazioni nel codice C++.

---

## ✉️ Contatti & Link
**Sviluppatore: Mattia Mungari**

LinkedIn: [![LinkedIn]](https://www.linkedin.com/in/mattiamungari)

Email: mungari.mattia@outlook.com