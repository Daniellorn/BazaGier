#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include "winsqlite/winsqlite3.h"


class Game {
private:
	unsigned int ID;
	std::string title;
	std::string developer;
	std::string review;

public:
	
	Game( unsigned int _ID , std::string& _title, std::string& _developer, std::string _review = "") :
		ID(_ID), title(_title), developer(_developer), review(_review) {}

	unsigned int getID() const { return ID; }
	std::string getTitle() const { return title; }
	std::string getDev() const { return developer; }
	std::string getReview() const { return review; }


	std::string getInfo() const
	{
		return std::to_string(ID) + " " + title + " " + developer + " " + review;
	}

	std::string toCSV() const
	{
		return std::to_string(ID) + ", \"" + title + "\", \"" + developer + "\", \"" + review + "\"";
	}

	bool mathesPhrase(const std::string& phrase)
	{
		return title.find(phrase) != std::string::npos ||
			developer.find(phrase) != std::string::npos ||
			review.find(phrase) != std::string::npos;
	}
};


class GameManager {
private:
	sqlite3* db;
	inline static std::string DataBaseName = "BazaGier.db";
	std::vector<Game> games;

	static int callback(void* data, int argc, char** argv, char** azColName)
	{
		std::vector<Game>* games = static_cast<std::vector<Game>*>(data);
		std::string s_id = argv[0] ? argv[0] : "0";
		unsigned int id = std::stoi(s_id);
		std::string title = argv[1] ? argv[1] : "";
		std::string developer = argv[2] ? argv[2] : "";
		std::string review = argv[3] ? argv[3] : "";
		games->emplace_back(id, title, developer, review);
		return 0;
	}

public:
	GameManager()
	{
		int rc = sqlite3_open(DataBaseName.c_str(), &db);

		if (rc)
		{
			std::cerr << "Nie mozna otworzyc bazy\n";
			exit(1);
		}

		const char* table = "CREATE TABLE IF NOT EXISTS Games ("
			"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
			"Tytul TEXT NOT NULL, "
			"Developer TEXT NOT NULL, "
			"Recenzja TEXT);";

		char* err_msg = 0;
		rc = sqlite3_exec(db, table, nullptr, nullptr, &err_msg);

		if (rc != SQLITE_OK)
		{
			std::cerr << "Blad tworzenia tabeli\n";
			sqlite3_free(err_msg);
			sqlite3_close(db);
			exit(1);
		}
	}


	~GameManager()
	{
		sqlite3_close(db);
	}

	void SafeToFile(const std::string& filename)
	{
		std::ofstream file(filename);

		if (!file)
		{
			std::cerr << "Blad otwarcia pliku\n";
			exit(1);
		}

		for (const auto& element : games)
		{
			file << element.getInfo() << std::endl;
		}
	}


	void LoadGames()
	{
		games.clear();
		std::string sqlSelect = "SELECT ID, Tytul, Developer, Recenzja FROM Games;";
		char* err_msg = 0;

		int rc = sqlite3_exec(db, sqlSelect.c_str(), callback, &games, &err_msg);

		if (rc != SQLITE_OK)
		{
			std::cerr << "Blad loadGames\n";
			sqlite3_free(err_msg);
			sqlite3_close(db);
			exit(1);
		}
	}

	void PrintMatchingGames(const std::string& phrase, int col)
	{
		std::vector<Game> arr;
		std::for_each(games.begin(), games.end(), [&phrase, &arr](Game& game)
			{
				if (game.mathesPhrase(phrase))
				{
					arr.push_back(game);
				}
			});

		if (col == 1)
		{
			for (auto& element : arr)
			{
				std::cout << element.getID() << ". "
					<< element.getTitle() << " "
					<< element.getDev() << " "
					<< element.getReview()
					<< std::endl;

			}
		}
		else if (col == 2)
		{
			std::sort(arr.begin(), arr.end(), [](Game& a, Game& b) { return a.getTitle() < b.getTitle(); });

			for (auto& element : arr)
			{
				std::cout << element.getID() << ". "
					<< element.getTitle() << " "
					<< element.getDev() << " "
					<< element.getReview()
					<< std::endl;

			}

		}
		else if (col == 3)
		{
			std::sort(arr.begin(), arr.end(), [](Game& a, Game& b) { return a.getDev() < b.getDev(); });
			for (auto& element : arr)
			{
				std::cout << element.getID() << ". "
					<< element.getTitle() << " "
					<< element.getDev() << " "
					<< element.getReview()
					<< std::endl;

			}
		}
		else if (col == 4)
		{
			std::sort(arr.begin(), arr.end(), [](Game& a, Game& b) { return a.getReview() < b.getReview(); });
			for (auto& element : arr)
			{
				std::cout << element.getID() << ". "
					<< element.getTitle() << " "
					<< element.getDev() << " "
					<< element.getReview()
					<< std::endl;

			}
		}
		else
		{
			std::cout << "Nieprawidlowy numer kolumny\n";
			exit(1);
		}
	}

	void List()
	{
		
		for (auto& element : games)
		{
			std::cout << element.getID() << ". "
				<< element.getTitle() << " "
				<< element.getDev() << " "
				<< element.getReview()
				<< std::endl;
		
		}
	}

	void List(int column)
	{
		if (column == 1)
		{
			List();
		}
		else if (column == 2)
		{
			std::sort(games.begin(), games.end(), [](Game& a, Game& b) { return a.getTitle() < b.getTitle(); });
			List();
		}
		else if (column == 3)
		{
			std::sort(games.begin(), games.end(), [](Game& a, Game& b) { return a.getDev() < b.getDev(); });
			List();
		}
		else if (column == 4)
		{
			std::sort(games.begin(), games.end(), [](Game& a, Game& b) { return a.getReview() < b.getReview(); });
			List();
		}
		else
		{
			std::cout << "Nieprawidlowy numer kolumny\n";
			exit(1);
		}
	}

	void ListCSV()
	{
		for (auto& element : games)
		{
			std::cout << element.toCSV() << std::endl;
		}
	}


	void Add(const Game game)
	{
		std::string sqlInsert = "INSERT INTO Games (Tytul, Developer) VALUES "
			"('" + game.getTitle() + "','" + game.getDev() + "');";

		char* err_msg = 0;
		int rc = sqlite3_exec(db, sqlInsert.c_str(), nullptr, nullptr, &err_msg);

		if (rc != SQLITE_OK)
		{
			std::cerr << "Blad dodawania do tabeli\n";
			sqlite3_free(err_msg);
		}
		else
		{
			std::cout << "Pomyslnie dodano gre\n";
			int lastID = sqlite3_last_insert_rowid(db);
			std::cout << "ID: " << lastID << std::endl;
		}
	}

	void Review(const std::string& rev, const std::string& id)
	{
		std::string sqlRev = "UPDATE Games SET Recenzja = '" + rev + "' WHERE ID = " + id + ";";

		char* err_msg = 0;
		int rc = sqlite3_exec(db, sqlRev.c_str(), nullptr, nullptr, &err_msg);

		if (rc != SQLITE_OK)
		{
			std::cerr << "Blad dodawania recenzji\n";
			sqlite3_free(err_msg);
		}
		else
		{
			std::cout << "Pomyslnie dodano recenzje\n";
		}
	}


	void Delete(const std::string& id)
	{
		std::string sqlDel = "DELETE FROM Games WHERE ID = " + id + ";";

		char* err_msg = 0;
		int rc = sqlite3_exec(db, sqlDel.c_str(), nullptr, nullptr, &err_msg);

		if (rc != SQLITE_OK)
		{
			std::cerr << "Blad usuwania wiersza\n";
			sqlite3_free(err_msg);
		}
		else
		{
			std::cout << "Pomyslnie usunieto wiersz\n";
		}
	}

};


enum Command {
	add,
	review,
	Delete,
	list,
	csv,
	search
};

Command getCommand(const std::string& cmd)
{
	if (cmd == "add") return Command::add;
	if (cmd == "review") return Command::review;
	if (cmd == "delete") return Command::Delete;
	if (cmd == "list") return Command::list;
	if (cmd == "csv") return Command::csv;
	if (cmd == "search") return Command::search;
}


int main(int argc, char** argv)
{
	GameManager GM;

	if (argc < 2)
	{
		std::cout << "Bledne argumenty\n";
		return 1;
	}

	Command cm = getCommand(argv[1]);
	

	switch (cm)
	{
	case Command::add:
		if (argc != 4)
		{
			std::cout << "Bledne argumenty, uzycie dla komendy add: /game add [tytul] [developer]\n";
			return 1;
		}
		{
			std::string title = argv[2];
			std::string dev = argv[3];
			Game game(0, title, dev);
			GM.Add(game);
			GM.LoadGames();
			GM.SafeToFile("PlikBazy.txt");
		}
		break;

	case Command::review:
		if (argc < 4)
		{
			std::cout << "Bledne argumenty, uzycie dla komendy review: /game review [ID] [tresc recenzji]";
			return 1;
		}
		{
			std::string id = argv[2];
			std::string rev = "";
			
			
			for (int i = 3; i < argc; i++)
			{
				if (i > 3)
				{
					rev += " ";
				}
				rev += argv[i];
			}

			GM.Review(rev, id);
			GM.LoadGames();
			GM.SafeToFile("PlikBazy.txt");
		}
		break;

	case Command::Delete:
		if (argc < 3)
		{
			std::cout << "Bledne argumenty, uzycie dla komendy delete: /game delete [ID]\n";
			return 1;
		}
		{
			std::string id = argv[2];
			GM.Delete(id);
			GM.LoadGames();
			GM.SafeToFile("PlikBazy.txt");
		}
		break;

	case Command::list:

		GM.LoadGames();
		if (argc == 2)
		{
			GM.List();
		}
		else if (argc == 3)
		{
			int col = std::stoi(argv[2]);
			GM.List(col);
		}
		else
		{
			std::cout << "Bledne argumenty, uzycie dla komendy list: /game list [opcjonalny numer kolumny]\n";
			return 1;
		}
		break;

	case Command::csv:
		
		if (argc != 2)
		{
			std::cout << "Bledne argumenty, uzycie dla komendy csv: /game csv\n";
			return 1;
		}
		GM.LoadGames();
		GM.ListCSV();
		break;

	case Command::search:

		GM.LoadGames();
		if (argc != 4)
		{
			std::cout << "Bledne argumenty, uzycie dla komendy search: /game search [fraza] [numer kolumny do posortowania]";
			return 1;
		}
		{
			std::string phrase = argv[2];
			int col = std::stoi(argv[3]);
			GM.PrintMatchingGames(phrase, col);
		}
		break;
	default:
		break;
	}
}