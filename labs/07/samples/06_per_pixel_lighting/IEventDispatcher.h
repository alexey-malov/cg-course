#pragma once

template <class ListenerType>
class IEventDispatcher
{
public:
	virtual void AddEventListener(ListenerType * pListener) = 0;
	virtual void RemoveEventListener(ListenerType * pListener) = 0;
protected:
	IEventDispatcher(void){}
	virtual ~IEventDispatcher(void){}
};

template <class Base, class ListenerType>
class CEventDispatcherImpl : public Base
{
public:
	// Добавляем указатель на слушателя событий в коллекцию слушателей
	virtual void AddEventListener(ListenerType * pListener)
	{
		m_listeners.insert(pListener);
	}

	// Удаляем указатель на слушателя событий из коллекции слушателей
	virtual void RemoveEventListener(ListenerType * pListener)
	{
		auto it = m_listeners.find(pListener);
		if (it != m_listeners.end())
		{
			m_listeners.erase(it);
		}
	}

	// Рассылаем всем слушателям событие, информация о котором
	// хранится в функциональном объекте event
	template <class EventType>
	void DispatchEvent(EventType const& event)const
	{
		// Создаем копию множества слушателей, т.к. в процессе рассылки
		// уведомления о событии обработчик может удалить слушателя из множества,
		// что сделает невалидными итераторы, передаваемые в for_each
		Listeners listeners(m_listeners);

		// оператор () объекта event будет вызыван столько раз, сколько
		// слушателей у диспетчера событий.
		// Параметром оператора () будет являться каждый из слушателей 
		// из коллекции listeners
		std::for_each(listeners.begin(), listeners.end(), event);
	}
private:
	typedef std::set<ListenerType *> Listeners;
	Listeners m_listeners;
};
