#include<iostream>
#include<string>
#include"sqlite3.h"
#include<ctime>

using namespace std;

int initialization();
int addUsers(string username, int ruolo);
string getCurrentDateTime();
int login(string username);
int addBreak(string username, int minuti);
int logout(string username, string reportTesto);
int pannelloAdmin();

int main() {
	initialization();
	addUsers("admin", 1);
	addUsers("DipendenteTest", 0);
	login("DipendenteTest");
	addBreak("DipendenteTest", 15);
	logout("DipendenteTest", "Report giornata test.");
	pannelloAdmin();
	return 0;
}

int initialization() {
	sqlite3* DB;
	int exit = 0;
	char* errMsg = 0;

	exit = sqlite3_open("dati_aziendali.db", &DB);

	if(exit){
		cerr << "Error open DB " << sqlite3_errmsg(DB) << endl;
		return -1;
	}else{
		cout << "Opened Database Successfully!" << endl;
	}

	const char* sql1_create =
		"CREATE TABLE IF NOT EXISTS utenti ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"username TEXT UNIQUE NOT NULL, "
		"ruolo INTEGER NOT NULL);";

	const char* sql2_create =
		"CREATE TABLE IF NOT EXISTS log_lavori ("
		"id_log INTEGER PRIMARY KEY, "
		"id_utente INTEGER NOT NULL , "
		"data TEXT NOT NULL, "
		"orario_login TEXT NOT NULL, "
		"orario_logout TEXT, "
		"minuti_pausa INTEGER DEFAULT 0, "
		"report TEXT, "
		"FOREIGN KEY (id_utente) REFERENCES utenti(id));";

	if(sqlite3_exec(DB, sql1_create, 0, 0, &errMsg) != SQLITE_OK){
		cerr << "Errore creazione tabella utenti: " << errMsg << endl;
		sqlite3_free(errMsg);
	}else{
		cout << "Tabella 'utenti' creata/verificata!" << endl;
	}

	if(sqlite3_exec(DB, sql2_create, 0, 0, &errMsg) != SQLITE_OK){
		cerr << "Errore creazione tabella log_lavori: " << errMsg << endl;
		sqlite3_free(errMsg);
	}else{
		cout << "Tabella 'log_lavori' creata/verificata!" << endl;
	}

	sqlite3_close(DB);
	return 0;
}

int addUsers(string username, int ruolo) {
	sqlite3* DB;
	char* errMsg = 0;
	sqlite3_open("dati_aziendali.db", &DB);

	string sql = "INSERT INTO utenti (username, ruolo) VALUES ('" + username + "', " + to_string(ruolo) + ");";

	int exit = sqlite3_exec(DB, sql.c_str(), 0, 0, &errMsg);

	if(exit != SQLITE_OK){
		cerr << "Errore inserimento utente: " << errMsg << endl;
		sqlite3_free(errMsg);
	}else{
		cout << "Utente " << username << " aggiunto con successo!" << endl;
	}

	sqlite3_close(DB);
	return exit;
}

string getCurrentDateTime() {
	time_t timestamp = time(0);
	struct tm tstruct;
	char buf[80];
	localtime_s(&tstruct, &timestamp);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);

	return string(buf);
}

int login(string username) {
	sqlite3* DB;
	sqlite3_stmt* stmt;
	int exit = sqlite3_open("dati_aziendali.db", &DB);

	string queryId = "SELECT id FROM utenti WHERE username = ?;";
	sqlite3_prepare_v2(DB, queryId.c_str(), -1, &stmt, NULL);
	sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

	int id_utente = -1;
	if(sqlite3_step(stmt) == SQLITE_ROW){
		id_utente = sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);

	if(id_utente == -1){
		cout << "Errore: Utente non trovato!" << endl;
		sqlite3_close(DB);
		return -1;
	}

	string timestamp = getCurrentDateTime();
	string dataOggi = timestamp.substr(0, 10);

	string sqlInsert = "INSERT INTO log_lavori (id_utente, data, orario_login, orario_logout) VALUES ("
		+ to_string(id_utente) + ", '" + dataOggi + "', '" + timestamp + "', 'In corso...');";

	char* errMsg = 0;
	exit = sqlite3_exec(DB, sqlInsert.c_str(), 0, 0, &errMsg);

	if(exit != SQLITE_OK){
		cerr << "Errore Login: " << errMsg << endl;
		sqlite3_free(errMsg);
	}else{
		cout << "Login effettuato per " << username << " alle " << timestamp << endl;
	}

	sqlite3_close(DB);
	return 0;
}

int addBreak(string username, int minuti) {
	sqlite3* DB;
	sqlite3_stmt* stmt;
	char* errMsg = 0;
	int exit = sqlite3_open("dati_aziendali.db", &DB);

	string queryId = "SELECT id FROM utenti WHERE username = ?;";
	sqlite3_prepare_v2(DB, queryId.c_str(), -1, &stmt, 0);
	sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

	int id_utente = -1;
	if(sqlite3_step(stmt) == SQLITE_ROW){
		id_utente = sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);

	if(id_utente == -1){
		cout << "Errorex: Utente non trovato per la pausa!" << endl;
		sqlite3_close(DB);
		return -1;
	}

	string sqlUpdate = "UPDATE log_lavori SET minuti_pausa = minuti_pausa + " + to_string(minuti) +
		" WHERE id_utente = " + to_string(id_utente) + " AND orario_logout = 'In corso...';";

	exit = sqlite3_exec(DB, sqlUpdate.c_str(), 0, 0, &errMsg);

	if(exit != SQLITE_OK){
		cerr << "Errore aggiornamento pausa: " << errMsg << endl;
		sqlite3_free(errMsg);
	}else{
		cout << minuti << " minuti di pausa aggiunti per " << username << "!" << endl;
	}

	sqlite3_close(DB);
	return exit;
}

int logout(string username, string reportTesto) {
	sqlite3* DB;
	sqlite3_stmt* stmt;
	char* errMsg = 0;
	int exit = sqlite3_open("dati_aziendali.db", &DB);

	string queryId = "SELECT id FROM utenti WHERE username = ?;";
	sqlite3_prepare_v2(DB, queryId.c_str(), -1, &stmt, 0);
	sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

	int id_utente = -1;
	if(sqlite3_step(stmt) == SQLITE_ROW){
		id_utente = sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);

	if(id_utente == -1){
		cout << "Errore: Utente non trovato per la chiusura!" << endl;
		sqlite3_close(DB);
		return -1;
	}

	string timestampFine = getCurrentDateTime();

	string sqlUpdate = "UPDATE log_lavori SET orario_logout = '" + timestampFine + "', report = '" + reportTesto +
		"' WHERE id_utente = " + to_string(id_utente) + " AND orario_logout = 'In corso...';";

	exit = sqlite3_exec(DB, sqlUpdate.c_str(), 0, 0, &errMsg);

	if(exit != SQLITE_OK){
		cerr << "Errore Logout: " << errMsg << endl;
		sqlite3_free(errMsg);
	}else{
		cout << "Giornata terminata per " << username << " alle " << timestampFine << endl;
		cout << "Report salvato: " << reportTesto << endl;
	}

	sqlite3_close(DB);
	return exit;
}

int pannelloAdmin() {
	sqlite3* DB;
	sqlite3_stmt* stmt;
	int exit = sqlite3_open("dati_aziendali.db", &DB);

	cout << "\n==========================================================================" << endl;
	cout << "                         PANNELLO AMMINISTRATORE                          " << endl;
	cout << "==========================================================================\n" << endl;

	string sql =
		"SELECT u.username, l.data, l.orario_login, l.orario_logout, l.minuti_pausa, "
		"ROUND(((strftime('%s', l.orario_logout) - strftime('%s', l.orario_login)) / 60.0) - l.minuti_pausa, 2) AS minuti_reali, "
		"l.report "
		"FROM log_lavori l "
		"JOIN utenti u ON l.id_utente = u.id "
		"WHERE l.orario_logout != 'In corso...' "
		"ORDER BY l.data DESC, l.orario_login DESC;";

	sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0);

	cout << "Dipendente      | Data       | Login               | Logout              | Pausa  | Min. Reali  | Report" << endl;
	cout << "---------------------------------------------------------------------------------------------------" << endl;

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		string user = (const char*)sqlite3_column_text(stmt, 0);
		string data = (const char*)sqlite3_column_text(stmt, 1);
		string login = (const char*)sqlite3_column_text(stmt, 2);
		string logout = (const char*)sqlite3_column_text(stmt, 3);
		int pausa = sqlite3_column_int(stmt, 4);
		double min_reali = sqlite3_column_double(stmt, 5);
		string report = (const char*)sqlite3_column_text(stmt, 6);

		cout << user << " | " << data << " | " << login << " | " << logout << " | "
			<< pausa << " min | " << min_reali << " min | " << report << endl;
	}

	cout << "==========================================================================\n" << endl;

	sqlite3_finalize(stmt);
	sqlite3_close(DB);
	return 0;
}