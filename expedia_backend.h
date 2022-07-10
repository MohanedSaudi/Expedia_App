

#ifndef EXPEDIA_BACKEND_H_
#define EXPEDIA_BACKEND_H_

#include "expedia_payments_api.h"
#include "expedia_payment_card.h"
#include "expedia_flights_api.h"
#include "expedia_utils.h"
#include "expedia_user.h"
#include "expedia_hotels_api.h"

class ExpediaBackend {
private:
	vector<IFlighsManager*> flights_managers;
	IPayment* payment_helper;

	vector<IHotelsManager*> hotels_managers;

public:
	ExpediaBackend(const ExpediaBackend&) = delete;
	void operator=(const ExpediaBackend&) = delete;

	ExpediaBackend() {
		flights_managers = FlightsFactory::GetManagers();
		payment_helper = PaymentFactory::GetPaymentHelper();
		hotels_managers = HotelsFactory::GetManagers();
	}
	vector<Flight> FindFlights(const FlightRequest &request) const {
		vector<Flight> flights;

		for (IFlighsManager* manager : flights_managers) {
			manager->SetFlightRequest(request);
			vector<Flight> more_flights = manager->SearchFlights();

			flights.insert(flights.end(), more_flights.begin(), more_flights.end());
		}
		return flights;
	}

	vector<HotelRoom> FindHotels(const HotelRequest &request) const {
		vector<HotelRoom> rooms;

		for (IHotelsManager* manager : hotels_managers) {
			manager->SetHotelRequest(request);
			vector<HotelRoom> more_rooms = manager->SearchHotelRooms();

			rooms.insert(rooms.end(), more_rooms.begin(), more_rooms.end());
		}
		return rooms;
	}

	bool ChargeCost(double cost, PaymentCard &payment_card) const {
		
		CreditCard* creditCard = nullptr;
		DebitCard* debitCard = nullptr;

		if ((creditCard = dynamic_cast<CreditCard*>(&payment_card)))
			payment_helper->SetUserInfo(payment_card.GetOwnerName(), "");
		else if ((debitCard = dynamic_cast<DebitCard*>(&payment_card))) {
			payment_helper->SetUserInfo(payment_card.GetOwnerName(), debitCard->GetBillingAddress());
		}
		payment_helper->SetCardInfo(payment_card.GetCardNumber(), payment_card.GetExpiryDate(), payment_card.GetSecurityCode());

		bool payment_status = payment_helper->MakePayment(cost);

		if (!payment_status)
			return false;	
		return true;
	}

	bool UnchargeCost(double cost, PaymentCard &payment_card) const {
		return true;
	}

	bool CancelReservation(const Reservation& reservation) {
		return true;
	}

	bool ConfirmReservation(const Reservation& reservation) {

		FlightReservation* flight = nullptr;
		Reservation* reservationCpy = reservation.Clone();

		if ((flight = dynamic_cast<FlightReservation*>(reservationCpy))) {
			string name = flight->GetFlight().GetAirlineName();
			IFlighsManager* mgr = FlightsFactory::GetManager(name);

			if (mgr != nullptr && mgr->ReserveFlight(*flight))
				return true;

			return false;
		}

		HotelReservation* hotel = nullptr;
		if ((hotel = dynamic_cast<HotelReservation*>(reservationCpy))) {
			string name = hotel->GetRoom().GetHotelName();
			IHotelsManager* mgr = HotelsFactory::GetManager(name);

			if (mgr != nullptr && mgr->ReserveRoom(*hotel))
				return true;

			return false;
		}

		ItineraryReservation* itiniary = nullptr;
		if ((itiniary = dynamic_cast<ItineraryReservation*>(reservationCpy))) {
			vector<Reservation*> confirmed_reservations;

			for (Reservation* sub_reservation : itiniary->GetReservations()) {
				bool is_confirmed = ConfirmReservation(*sub_reservation);

				if (is_confirmed)
					confirmed_reservations.push_back(sub_reservation);
				else {
					for (Reservation* conf_reservation : confirmed_reservations)
						CancelReservation(*conf_reservation);
					return false;
				}
			}
		} else
			assert(false);
		delete reservationCpy;
		reservationCpy = nullptr;
		return true;
	}
};

#endif 
