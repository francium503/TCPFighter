#pragma once

template <typename T>
class List
{
public:
	struct Node
	{
		T _data;
		Node *_Prev;
		Node *_Next;
	};

	class iterator
	{
	private:
		Node *_node;
	public:
		iterator(Node *node = nullptr);
		iterator operator ++(int);
		iterator operator ++ ();
		T& operator *();
		bool operator !=(const iterator& rValue);
		bool operator ==(const iterator& rValue);
		iterator operator --(int);
		
		
	protected:
		void eraseSelf();
		Node* getNode();

		friend class List;
	};

public:
	List();
	~List();

	iterator begin()
	{
		iterator iter(&_head);

		return iter;
	}
	iterator end()
	{
		iterator iter(&_tail);

		return iter;
	}

	void push_front(T data);
	void push_back(T data);
	void clear();
	int size() { return _size; };
	bool empty()
	{
		if (_size == 0)
		{
			return true;
		}
		return false;
	};
	iterator erase(iterator iter);

private:
	int _size;
	Node _head;
	Node _tail;
};


template <typename T>
List<T>::iterator::iterator(Node* node)
{
	_node = node;
}

template <typename T>
typename List<T>::iterator List<T>::iterator::operator++(int)
{
	iterator *tmp = this;

	_node = _node->_Next;

	return *tmp;
}

template <typename T>
typename List<T>::iterator List<T>::iterator::operator++()
{
	_node = _node->_Next;
	return *this;
}

template <typename T>
T& List<T>::iterator::operator*()
{
	return _node->_data;
}

template <typename T>
bool List<T>::iterator::operator!=(const iterator& rValue)
{
	if (_node == rValue._node)
		return false;
	if (_node->_Prev == rValue._node->_Prev && _node->_Next == rValue._node->_Next)
		return false;
	return true;
}

template <typename T>
bool List<T>::iterator::operator==(const iterator& rValue)
{
	if (_node != rValue._node)
		return false;

	if (_node->_Prev != rValue._node->_Prev || _node->_Next != rValue._node->_Next)
		return false;
	return true;
}

template <typename T>
typename List<T>::iterator List<T>::iterator::operator--(int)
{
	iterator *tmp = this;

	if(_node->_Prev != nullptr)
		_node = _node->_Prev;

	return *tmp;
}

template <typename T>
typename List<T>::Node* List<T>::iterator::getNode()
{
	return this->_node;
}

template <typename T>
void List<T>::iterator::eraseSelf()
{
	if (this->_node->_Prev == nullptr)
	{
		this->_node->_Next->_Prev = nullptr;
		return;
	}
	else if (this->_node->_Next == nullptr)
	{
		this->_node->_Prev->_Next = nullptr;
		return;
	}


	this->_node->_Prev->_Next = this->_node->_Next;
	this->_node->_Next->_Prev = this->_node->_Prev;
}

template <typename T>
List<T>::List() : _size(0)
{
	_head._Prev = nullptr;
	_head._Next = nullptr;

	_tail = _head;
}

template <typename T>
List<T>::~List()
{

}

template <typename T>
void List<T>::push_front(T data)
{
	if (_size == 0)
	{
		_head._data = data;

		_head._Next = &_tail;
		_tail._Prev = &_head;
		_tail._Next = nullptr;

		_size++;
		return;
	}

	Node *tmp = new Node;
	tmp->_data = _head._data;
	tmp->_Prev = &_head;
	tmp->_Next = _head._Next;
	_head._Next = tmp;

	_head._data = data;

	_size++;
}

template <typename T>
void List<T>::push_back(T data)
{
	if (_size == 0)
	{
		_head._data = data;

		_head._Next = &_tail;
		_tail._Prev = &_head;
		_tail._Next = nullptr;

		_size++;
		return;
	}

	Node *tmp = new Node;
	tmp->_data = data;
	_tail._Prev->_Next = tmp;
	tmp->_Prev = _tail._Prev;
	tmp->_Next = &_tail;
	_tail._Prev = tmp;

	_size++;
}

template <typename T>
void List<T>::clear()
{
	_head._Prev = nullptr;
	_head._Next = nullptr;

	_tail = _head;

	_size = 0;
}

template <typename T>
typename List<T>::iterator List<T>::erase(iterator iter)
{
	Node* tmp = iter.getNode();

	if (tmp->_Prev == nullptr)
	{
		tmp = tmp->_Next;
		tmp->_Prev = nullptr;
		_head = *tmp;
		_size--;
		return iter;
	}

	if (tmp->_Next == nullptr)
	{
		tmp = tmp->_Prev;
		tmp->_Next = nullptr;
		_tail = *tmp;
		_size--;
		return iter;
	}

	Node* next = tmp->_Next;
	Node* prev = tmp->_Prev;

	prev->_Next = next;
	next->_Prev = prev;

	_size--;

	tmp = next;

	return iter;
}