

#ifndef EXPEDIA_USER_MGR_H_
#define EXPEDIA_USER_MGR_H_

#include <map>
using namespace std;

#include "expedia_payment_card.h"
#include "expedia_user.h"
#include "expedia_utils.h"


class UsersManager {
protected:
	User* current_user = nullptr;
	map<string, User*> userame_userobject_map;

	void FreeLoadedData() {
		for (auto pair : userame_userobject_map) {
			delete pair.second;
		}
		userame_userobject_map.clear();
		current_user = nullptr;
	}

public:

	UsersManager() {
	}

	~UsersManager() {
		cout << "Destructor: UsersManager\n";
		FreeLoadedData();
	}

	UsersManager(const UsersManager&) = delete;
	void operator=(const UsersManager&) = delete;

	void LoadDatabase() {
		cout << "UsersManager: LoadDatabase\n";

		FreeLoadedData();

		Admin* admin_user = new Admin();
		admin_user->SetUserName("mostafa");
		admin_user->SetPassword("111");
		admin_user->SetEmail("mostafa@gmail.com");
		admin_user->SetName("mostafa Saad Ibrahim");
		userame_userobject_map[admin_user->GetUserName()] = admin_user;

		Customer* customrUser = new Customer();
		customrUser->SetUserName("asmaa");
		customrUser->SetPassword("222");
		customrUser->SetEmail("asmaa@gmail.com");
		customrUser->SetName("Asmaa Saad Ibrahim");

		DebitCard* debitCard = new DebitCard();
		debitCard->SetOwnerName(customrUser->GetName());
		debitCard->SetCardNumber("11-22-33-44");
		debitCard->SetExpiryDate("11/20");
		debitCard->SetSecurityCode(111);
		debitCard->SetBillingAddress("111 hello st, BC, Canada");
		customrUser->AddPaymentCard(*debitCard);

		CreditCard* creditCard = new CreditCard();
		creditCard->SetOwnerName(customrUser->GetName());
		creditCard->SetCardNumber("22-11-03-44");
		creditCard->SetExpiryDate("11/25");
		creditCard->SetSecurityCode(1117);
		customrUser->AddPaymentCard(*creditCard);
		userame_userobject_map[customrUser->GetUserName()] = customrUser;

	}

	void AccessSystem() {
		int choice = ShowReadMenu( { "Login", "Customer Sign Up" });
		if (choice == 1)
			DoLogin();
		else
			DoSignUp();
	}

	void DoLogin() {
		LoadDatabase();

		while (true) {
			string user_name, pass;
			cout << "Enter user user_name & password: ";
			cin >> user_name >> pass;

			if (!userame_userobject_map.count(user_name)) {
				cout << "\nInvalid user user_name or password. Try again\n\n";
				continue;
			}
			User* user_exist = userame_userobject_map.find(user_name)->second;

			if (pass != user_exist->GetPassword()) {
				cout << "\nInvalid user user_name or password. Try again\n\n";
				continue;
			}
			current_user = user_exist;
			break;
		}
	}

	void DoSignUp() {
		string user_name;
		while (true) {
			cout << "Enter user name. (No spaces): ";
			cin >> user_name;

			if (userame_userobject_map.count(user_name))
				cout << "Already used. Try again\n";
			else
				break;
		}
		current_user = new Customer();
		current_user->Read(user_name);
		userame_userobject_map[current_user->GetUserName()] = current_user;
	}

	User* GetCurrentUser() const {
		
		return current_user;
	}
};



#endif 

