#include <iostream>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <map>
#include <time.h>
#include <stdlib.h>

#pragma comment(lib, "cpprest_2_10")

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace std;
using namespace json;

map<wstring, int> Sessions;
int g_SessionID = 0;

int m_Health = 100;

int main()
{
	srand(time(0));

	void Handle_Get(http_request request);
	void Handle_Post(http_request request);
	void Handle_GameOver(http_request request);
	void Handle_Rand(http_request request);
	void Handle_TakeDamage(http_request request);


	http_listener listener(L"http://localhost:8777/SLCGame311");

	listener.support(methods::GET, Handle_Get);
	listener.support(methods::GET, Handle_GameOver);
	listener.support(methods::GET, Handle_Rand);
	listener.support(methods::POST, Handle_Post);
	listener.support(methods::POST, Handle_TakeDamage);

	try
	{
		listener.open().then([&listener]()
			{
				wcout << ("\nListening::\n") << endl;
				wcout << listener.uri().to_string().c_str() << endl;
			})
			.wait();
	}
	catch (exception const& e)
	{
		wcout << e.what() << endl;
	}

	while (true);

	return 0;
}

void Handle_TakeDamage(http_request request)
{
	cout << "\nhandle TakeDamage\n";

	wstring APIPath = request.absolute_uri().to_string();
	wcout << "\nAPI PATH: " << APIPath << endl;

	if (wcscmp(APIPath.c_str(), L"/SLCGame311/TakeDamage/") == 0)
	{
		if (request.headers().has(L"Dmg"))
		{
			value ReqJSONData = value::object();

			request.extract_json().then([&ReqJSONData](pplx::task<value>task)
			{
				ReqJSONData = task.get();
			}).wait();

			if (ReqJSONData.has_field(L"Damage"))
			{
				int dmg = ReqJSONData.at(L"Damage").as_number().to_int64();

				m_Health -= dmg;

				value JSON_HP_Obj = value::object();

				JSON_HP_Obj[L"Health"] = web::json::value::number(m_Health);

				request.reply(status_codes::OK, JSON_HP_Obj);
			}
			else
			{
				request.reply(status_codes::FailedDependency, "No Damage Key in JSON Object");
			}
		}
		else
		{
			request.reply(status_codes::FailedDependency, "Header does not include \"Dmg\"");
		}

	}
	else
	{
		request.reply(status_codes::BadRequest, "Bad Request");
	}

	request.reply(status_codes::OK, "Status OK");
}

void Handle_Rand(http_request request)
{
	cout << " get request made!" << endl;

	wstring APIPath = request.absolute_uri().to_string();
	wcout << "\nAPI PATH: " << APIPath << endl;

	if (wcscmp(APIPath.c_str(), L"/SLCGame311/Rand/") == 0)
	{
		value JSONObj = value::object();

		auto random = rand();

		JSONObj[L"RandVal"] = web::json::value::number(random);

		request.reply(status_codes::OK, JSONObj);
	}
	else
	{
		request.reply(status_codes::BadRequest, "Bad Request");
	}
}

void Handle_GameOver(http_request request)
{
	cout << " get request made!" << endl;

	wstring APIPath = request.absolute_uri().to_string();
	wcout << "\nAPI PATH: " << APIPath << endl;

	if (wcscmp(APIPath.c_str(), L"/SLCGame311/GameOver/") == 0)
	{
		value JSONObj = value::object();
		JSONObj[L"GameOver"] = web::json::value::boolean(true);

		request.reply(status_codes::OK, JSONObj);
	}
	else
	{
		request.reply(status_codes::BadRequest, "Bad Request");
	}
}

void Handle_Get(http_request request)
{
	cout << " get request made!" << endl;

	wstring APIPath = request.absolute_uri().to_string();
	wcout << "\nAPI PATH: " << APIPath << endl;

	if (wcscmp(APIPath.c_str(), L"/SLCGame311/PlayerDied/") == 0)
	{
		if (request.headers().has(L"UserID"))
		{
			value JSONObj = value::object();
			http_headers reqHeaders = request.headers();

			JSONObj[L"UserID"] = value::string(reqHeaders[L"UserID"]);

			request.reply(status_codes::FailedDependency, JSONObj);
		}
		else
		{
			request.reply(status_codes::FailedDependency, "Failed Dependency");
		}
	}
	else
	{
		request.reply(status_codes::BadRequest, "Bad Request");
	}

	request.reply(status_codes::OK, "Status OK");
}

void Handle_Post(http_request request)
{
	cout << "\nhandle POST\n";

	wstring APIPath = request.absolute_uri().to_string();
	wcout << "\nAPI PATH: " << APIPath << endl;

	if (wcscmp(APIPath.c_str(), L"/SLCGame311/Login/") == 0)
	{
		value ReqJSONData = value::object();

		request.extract_json().then([&ReqJSONData](pplx::task<value>task)
			{
				ReqJSONData = task.get();
			}).wait();

			if (ReqJSONData.has_string_field(L"Name"))
			{
				utility::string_t Name = ReqJSONData.at(L"Name").as_string();

				g_SessionID++;

				wstring initials = Name;

				if (Sessions[initials] != NULL)
				{
					Name.append(L" Welcome Back!");
				}
				else
				{
					Sessions[initials] = g_SessionID;

					Name.append(L" Welcome to the server. Since this is your first time, please see your token attached.");
				}

				request.reply(status_codes::OK, Name);
			}
			else
			{
				request.reply(status_codes::FailedDependency, "Failed Dependency");
			}
	}
	else
	{
		request.reply(status_codes::BadRequest, "Bad Request");
	}

	request.reply(status_codes::OK, "Status OK");
}