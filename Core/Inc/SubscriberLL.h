#pragma once

#include "ProcessRequest.h"

#include <functional>

class Subscriber {
private:
	Subscriber * next = nullptr;
	ProcessRequest processRequest;
public:
	Subscriber(std::function<void(void)> callback) :
			processRequest(callback)
	{

	}

	ProcessRequest & getProcessRequest() {
		return processRequest;
	}

	void setNext(Subscriber * next) {
		this->next = next;
	}

	Subscriber * getNext() {
		return next;
	}

	static bool isSubscriberInSubscriberList(Subscriber * sub, Subscriber * list) {
		Subscriber * current = list;
		while (current != nullptr) {
			if (current == sub) {
				return true;
			}
			current = current->getNext();
		}
		return false;
	}

	static void addSubscription(Subscriber * sub, Subscriber ** list_p) {
		if (Subscriber::isSubscriberInSubscriberList(sub, *list_p)) {
			//already in list
			return;
		}
		// not yet in list; add it to the front.
		sub->setNext(*list_p);
		*list_p = sub;
	}

};
