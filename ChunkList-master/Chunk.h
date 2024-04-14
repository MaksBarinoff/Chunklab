#pragma once
#include <iterator>
#include <memory>
#include <list>
#include <algorithm>
#include <exception>
#include <compare>
#include <iostream>


namespace fefu_laboratory_two {
	template <typename T>
	class Allocator {
	public:
		using value_type = T;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;


		constexpr Allocator() noexcept {};

		constexpr Allocator(const Allocator& other) noexcept = default;

		template <class U>
		constexpr Allocator(const Allocator<U>& other) noexcept {};

		~Allocator() = default;

		pointer allocate(size_type N) {
			pointer ptr = static_cast<pointer>(::operator new(sizeof(value_type) * N));
			if (ptr != nullptr) {
				return ptr;
			}
			else {
				throw std::bad_alloc();
			}
		}

		void deallocate(pointer p, const size_t N) noexcept {
			static_cast<void>(N);
			free(p);
		}
	};

	template <typename ValueType, typename Allocator = Allocator<ValueType>>
	class Chunk {
	public:
		using size_type = std::size_t;
		ValueType* list = nullptr;
		Chunk* prev = nullptr;
		Chunk* next = nullptr;
		int chunk_size = 0;
		int num_of_elements = 0;
		Allocator allocator;

		Chunk(int N) {
			list = allocator.allocate(N);
			chunk_size = N;
		}

		ValueType* get_data() {
			ValueType* data = allocator.allocate(chunk_size);
			for (int i = 0; i < chunk_size; i++)
				data[i] = list[i];
			return data;
		}

		ValueType* begin() {
			return list;
		}

		ValueType* end() {
			return list + num_of_elements;
		}

		void resize(size_type new_size) {
			if (new_size < 0)
				throw std::invalid_argument("New size cannot be negative");

			if (new_size == chunk_size)
				return;

			size_type min_v = (chunk_size > new_size) ? new_size : chunk_size;
			ValueType* new_list = allocator.allocate(new_size);
			for (int i = 0; i < min_v; ++i) {
				new_list[i] = list[i];
			}

			if (num_of_elements > new_size)
				num_of_elements = new_size;

			allocator.deallocate(list, chunk_size);
			list = new_list;
			chunk_size = new_size;
		}

		void resize(size_type new_size, const ValueType& value) {
			if (new_size < 0)
				throw std::invalid_argument("New size cannot be negative");

			if (new_size == chunk_size)
				return;
			
			ValueType* new_list = allocator.allocate(new_size);
			if (new_size <= chunk_size) {
				for (int i = 0; i < new_size; ++i) {
					new_list[i] = list[i];
				}
			}
			else {
				for (int i = 0; i < chunk_size; ++i) {
					new_list[i] = list[i];
				}

				for (int i = chunk_size; i < new_size; ++i) {
					new_list[i] = value;
				}
			}


			if (num_of_elements > new_size)
				num_of_elements = new_size;

			allocator.deallocate(list, chunk_size);
			list = new_list;
			chunk_size = new_size;
			num_of_elements = new_size;
		}
	};

	template<typename ValueType>
	class ChunkListInterface {
	public:
		virtual ~ChunkListInterface() {}
		virtual ValueType& at(size_t index) = 0;
		virtual const ValueType& at(size_t index) const = 0;
		virtual size_t size() const noexcept = 0;
		virtual ValueType& operator[](std::ptrdiff_t n) = 0;
		virtual const ValueType& operator[](std::ptrdiff_t n) const = 0;
	};

	template <typename ValueType>
	class ChunkList_iterator {
	protected:
		int elem_index = 0;
		ChunkListInterface<ValueType>* list = nullptr;
		ValueType* current_value = nullptr;
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = ValueType;
		using difference_type = std::ptrdiff_t;
		using pointer = ValueType*;
		using reference = ValueType&;

		int get_index() { return elem_index; };

		constexpr ChunkList_iterator() noexcept = default;

		ChunkList_iterator(ChunkListInterface<ValueType>* chunk, int index, ValueType* value) :
			list(chunk),
			elem_index(index),
			current_value(value)
		{
		};

		ChunkList_iterator(const ChunkList_iterator<ValueType>& other)
		{
			list = other.list;
			elem_index = other.elem_index;
			current_value = other.current_value;
		};

		ChunkList_iterator& operator=(const ChunkList_iterator& other) {
			list = other.list;
			elem_index = other.elem_index;
			current_value = other.current_value;
		};

		~ChunkList_iterator() = default;

		void swap(ChunkList_iterator<ValueType>& a, ChunkList_iterator<ValueType>& b) {
			std::swap(a.list, b.list);
			std::swap(a.current_value, b.current_value);
			std::swap(a.elem_index, b.elem_index);
		};

		friend bool operator==(const ChunkList_iterator<ValueType>& lhs,
			const ChunkList_iterator<ValueType>& rhs) {
			return lhs.current_value == rhs.current_value;
		};

		friend bool operator!=(const ChunkList_iterator<ValueType>& lhs,
			const ChunkList_iterator<ValueType>& rhs) {
			return !(lhs.current_value == rhs.current_value);
		};

		reference operator*() const { return *current_value; };
		pointer operator->() const { return current_value; };

		ChunkList_iterator operator++(int) {
			if (elem_index + 1 == list->size())
				return ChunkList_iterator();
			elem_index++;
			current_value = &list->at(elem_index);
			return *this;
		};
		ChunkList_iterator operator--(int) {
			elem_index--;
			if (elem_index <= -1)
				return *this;

			current_value = &list->at(elem_index);
			return *this;
		};

		ChunkList_iterator& operator++() {
			if (elem_index + 1 == list->size()) {
				current_value = nullptr;
				list = nullptr;
				elem_index = 0;
				return *this;
			}
			elem_index++;
			current_value = &list->at(elem_index);
			return *this;
		};

		ChunkList_iterator& operator--() {
			elem_index--;
			if (elem_index <= -1)
				return *this;

			current_value = &list->at(elem_index);
			return *this;
		};

		ChunkList_iterator operator+(const difference_type& n) const {
			return ChunkList_iterator(list, elem_index + n, &list->at(elem_index + n));
		};


		ChunkList_iterator operator-(const difference_type& n) const {
			return ChunkList_iterator(list, elem_index - n, &list->at(elem_index - n));
		};

		ChunkList_iterator& operator+=(const difference_type& n) {
			elem_index += n;
			current_value = &list->at(elem_index);
			return *this;
		};

		ChunkList_iterator& operator-=(const difference_type& n) {
			elem_index -= n;
			current_value = &list->at(elem_index);
			return *this;
		};


		reference operator[](const difference_type& n) {
			return list[n];
		};

		friend bool operator<(const ChunkList_iterator<ValueType>& lhs,
			const ChunkList_iterator<ValueType>& rhs) {
			return lhs.elem_index < rhs.elem_index;
		};
		friend bool operator<=(const ChunkList_iterator<ValueType>& lhs,
			const ChunkList_iterator<ValueType>& rhs) {
			return lhs.elem_index <= rhs.elem_index;
		};
		friend bool operator>(const ChunkList_iterator<ValueType>& lhs,
			const ChunkList_iterator<ValueType>& rhs) {
			return lhs.elem_index > rhs.elem_index;
		};
		friend bool operator>=(const ChunkList_iterator<ValueType>& lhs,
			const ChunkList_iterator<ValueType>& rhs) {
			return lhs.elem_index >= rhs.elem_index;
		};
	};

	template <typename ValueType>
	class ChunkList_const_iterator {
	public:
		int elem_index = 0;
		const ChunkListInterface<ValueType>* list = nullptr;
		const ValueType* current_value = nullptr;

		using iterator_category = std::random_access_iterator_tag;
		using value_type = ValueType;
		using difference_type = std::ptrdiff_t;
		using const_pointer = const ValueType*;
		using const_reference = const ValueType&;

		const int get_index() const { return elem_index; };

		ChunkList_iterator<ValueType> constIteratorToIterator() {
			return ChunkList_iterator<ValueType>(const_cast<ChunkListInterface<ValueType>*>(list), elem_index, const_cast<ValueType*>(current_value));
		}

		constexpr ChunkList_const_iterator() noexcept = default;

		ChunkList_const_iterator(ChunkListInterface<ValueType>* chunk, int index, ValueType* value) :
			list(chunk),
			elem_index(index),
			current_value(value) 
		{
		};

		ChunkList_const_iterator(const ChunkListInterface<ValueType>* chunk, int index, const ValueType* value) :
			list(chunk),
			elem_index(index),
			current_value(value) 
		{	
		};

		ChunkList_const_iterator(const ChunkList_const_iterator<ValueType>& other) {
			list = other.list;
			elem_index = other.elem_index;
			current_value = other.current_value;
		};

		ChunkList_const_iterator& operator=(const ChunkList_const_iterator&) = default;

		~ChunkList_const_iterator() = default;

		void swap(ChunkList_const_iterator<ValueType>& a, ChunkList_const_iterator<ValueType>& b) {
			std::swap(a.list, b.list);
			std::swap(a.current_value, b.current_value);
			std::swap(a.elem_index, b.elem_index);
		};

		friend bool operator==(const ChunkList_const_iterator<ValueType>& lhs,
			const ChunkList_const_iterator<ValueType>& rhs) {
			return lhs.current_value == rhs.current_value;
		};
		friend bool operator!=(const ChunkList_const_iterator<ValueType>& lhs,
			const ChunkList_const_iterator<ValueType>& rhs) {
			return lhs.current_value != rhs.current_value;
		};

		const_reference operator*() const { return *current_value; };
		const_pointer operator->() const { return current_value; };
		const_reference operator[](const difference_type& n) {
			return list[n];
		};

		ChunkList_const_iterator operator++(int) {
			if (elem_index + 1 == list->size())
				return ChunkList_const_iterator();
			elem_index++;
			current_value = &list->at(elem_index);
			return *this;
		};

		ChunkList_const_iterator operator--(int) {
			return ChunkList_iterator<ValueType>::operator--(0);
		};

		ChunkList_const_iterator& operator++() {
			if (elem_index + 1 == list->size()) {
				current_value = nullptr;
				list = nullptr;
				elem_index = 0;
				return *this;
			}
			elem_index++;
			current_value = &list->at(elem_index);
			return *this;
		};

		ChunkList_const_iterator& operator--() {
			if (elem_index - 1 == -1)
				throw std::exception();
			elem_index--;
			current_value = &list->at(elem_index);
			return *this;
		};

		ChunkList_const_iterator operator+(const difference_type& n) const {
			return ChunkList_const_iterator(list, elem_index + n, &list->at(elem_index + n));
		};


		ChunkList_const_iterator operator-(const difference_type& n) const {
			return ChunkList_const_iterator(list, elem_index - n, &list->at(elem_index - n));
		};

		ChunkList_const_iterator& operator+=(const difference_type& n) {
			elem_index += n;
			current_value = &list->at(elem_index);
			return *this;
		};

		ChunkList_const_iterator& operator-=(const difference_type& n) {
			elem_index -= n;
			current_value = &list->at(elem_index);
			return *this;
		};

		friend bool operator<(const ChunkList_const_iterator<ValueType>& lhs,
			const ChunkList_const_iterator<ValueType>& rhs) {
			return lhs.elem_index < rhs.elem_index;
		};
		friend bool operator<=(const ChunkList_const_iterator<ValueType>& lhs,
			const ChunkList_const_iterator<ValueType>& rhs) {
			return lhs.elem_index <= rhs.elem_index;
		};
		friend bool operator>(const ChunkList_const_iterator<ValueType>& lhs,
			const ChunkList_const_iterator<ValueType>& rhs) {
			return lhs.elem_index > rhs.elem_index;
		};
		friend bool operator>=(const ChunkList_const_iterator<ValueType>& lhs,
			const ChunkList_const_iterator<ValueType>& rhs) {
			return lhs.elem_index >= rhs.elem_index;
		};
	};

	template <typename T, int N, typename Allocator = Allocator<T>>
	class ChunkList : public ChunkListInterface<T> {
	protected:
		Chunk<T, Allocator>* first_chunk = nullptr;
		int list_size = 0;
		int chunk_size = N;
	public:

		using value_type = T;
		using allocator_type = Allocator;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = typename std::allocator_traits<Allocator>::pointer;
		using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
		using iterator = ChunkList_iterator<value_type>;
		using const_iterator = ChunkList_const_iterator<value_type>;

		ChunkList() : first_chunk(new Chunk<value_type>(N)) {};


		ChunkList(size_type count, const T& value = T(), const Allocator& alloc = Allocator())
			: first_chunk(new Chunk<value_type, allocator_type>(N))
		{
			int i = 0;
			auto current_chunk = first_chunk;
			while (i < count) {
				current_chunk->allocator = alloc;
				for (int j = 0; j < N; j++) {
					current_chunk->list[j] = value;
					i++;
					if (i == count) {
						break;
					}
				}
				if (i < count) {
					current_chunk->next = new Chunk<value_type, allocator_type>(N);
					current_chunk = current_chunk->next;
				}
			}
		};

		explicit ChunkList(size_type count, const Allocator& alloc = Allocator())
			: first_chunk(new Chunk<value_type, allocator_type>(N))
		{
			int i = 0;
			Chunk<value_type, allocator_type>* current_chunk = first_chunk;
			while (i < count) {
				current_chunk->allocator = alloc;
				for (int j = 0; j < N; j++) {
					current_chunk->list[j] = T();
					i++;
					if (i == count)
						break;
				}
				if (i < count) {
					current_chunk->next = new Chunk<value_type, allocator_type>(N);
					current_chunk = current_chunk->next;
				}
			}
		};

		template <class InputIt>
		ChunkList(InputIt first, InputIt last, const Allocator& alloc = Allocator()) 
			: first_chunk(new Chunk<value_type, allocator_type>(N))
		{
			Chunk<value_type, allocator_type>* current_chunk = first_chunk;
			auto it = first;
			int i = 0;
			while (it != last) {
				current_chunk->allocator = alloc;	
				for (i = 0; i < N && it != last; ++i, ++it) {
					current_chunk->list[i] = *it;
					current_chunk->num_of_elements++;
					list_size++;
					if (it == last)
						break;
				}
				if (it != last) {
					current_chunk->next = new Chunk<value_type, allocator_type>(N);
					current_chunk = current_chunk->next;
				}
			}
		};

		ChunkList(const ChunkList& other)  {
			first_chunk = new Chunk<value_type>(N);
			Chunk<value_type>* old_list = other.first_chunk;
			Chunk<value_type>* new_list = first_chunk;

			while (old_list != nullptr) {
				new_list->list = old_list->get_data();
				new_list->chunk_size = old_list->chunk_size;
				if (old_list->next != nullptr) {
					new_list->next = new Chunk<value_type>(N);
					Chunk<value_type>* tmp = new_list;
					new_list = new_list->next;
					new_list->prev = tmp;
				}
				old_list = old_list->next;
			}
			list_size = other.list_size;
		};

		ChunkList(const ChunkList& other, const Allocator& alloc) {
			this = ChunkList(other);
			Chunk<value_type, allocator_type>* current_chunk = this->first_chunk;
			while (current_chunk != nullptr) {
				current_chunk->allocator = alloc;
				current_chunk = current_chunk->next;
			}
		};


		ChunkList(ChunkList&& other) {
			first_chunk = std::move(other.first_chunk);
			list_size = std::move(other.list_size);
			other.clear();
		};


		ChunkList(ChunkList&& other, const Allocator& alloc) {
			this = ChunkList(other);
			Chunk<value_type, allocator_type>* current_chunk = first_chunk;
			while (current_chunk != nullptr) {
				current_chunk->allocator = alloc;
				current_chunk = current_chunk->next;
			}
		};

		ChunkList(std::initializer_list<T> init, const Allocator& alloc = Allocator())
			: first_chunk(new Chunk<value_type, allocator_type>(N))
		{
			first_chunk->allocator = alloc;
			auto it = init.begin();
			Chunk<value_type, allocator_type>* current_chunk = first_chunk;
			int count = 0;
			while (it != init.end()) {
				current_chunk->list[count] = *it;
				++count;
				++it;
				if (count == N) {
					current_chunk->next = new Chunk<value_type, allocator_type>(N);
					current_chunk = current_chunk->next;
					count = 0;
				}
			}
			list_size = init.size();
		}

		~ChunkList() {
			clear();
		};

		ChunkList& operator=(const ChunkList& other) {
			(this) = new ChunkList(other);
			return (*this);
		};

		ChunkList& operator=(ChunkList&& other) {
			first_chunk = std::move(other.first_chunk);
			list_size = std::move(other.list_size);
			other.clear();

			return *this;
		};

		ChunkList& operator=(std::initializer_list<T> ilist) {
			clear();
			auto it = ilist.begin();
			Chunk* current_chunk = first_chunk;
			int count = 0;
			for (const auto& elem : ilist) {
				current_chunk->list[count] = elem;
				++count;
				if (count == N) {
					if (current_chunk->next == nullptr) {
						current_chunk->next = new Chunk<value_type, allocator_type>(N);
						current_chunk->next->prev = current_chunk;
					}
					current_chunk = current_chunk->next;
					count = 0;
				}
			}
			list_size = ilist.size();
			return *this;
		}

		void assign(size_type count, const T& value) {
			if (count < 0)
				throw std::out_of_range("Count argument must be non-negative");
			clear();
			for (int i = 0; i < count; i++)
				push_back(value);
			list_size = count;
		};

		template <class InputIt>
		void assignIt(InputIt first, InputIt last) {
			clear();
			for (InputIt it = first; it != last; ++it) {
				push_back(*it);
			}
		}

		void assign(std::initializer_list<T> ilist) {
			clear();
			for (const auto& value : ilist) {
				push_back(value);
			}
		}

		allocator_type get_allocator() const noexcept {
			return first_chunk->allocator;
		};

		Chunk<value_type, allocator_type>* last_chunk() {
			Chunk<value_type, allocator_type>* current_chunk = first_chunk;
			while (current_chunk->next != nullptr)
				current_chunk = current_chunk->next;
			return current_chunk;
		}

		reference at(size_type pos) {
			if (pos >= max_size() || pos < 0) {
				throw std::out_of_range("Out of range");
			}
			int chunk_index = pos / chunk_size;
			int elemnt_index = pos % chunk_size;

			Chunk<value_type, allocator_type>* curr_chunk = first_chunk;
			while (chunk_index > 0) {
				curr_chunk = curr_chunk->next;
				chunk_index--;
			}
			return curr_chunk->list[elemnt_index];
		};

		const_reference at(size_type pos) const {
			if (pos >= max_size() || pos < 0) {
				throw std::out_of_range("Out of range");
			}
			int chunk_index = pos / N;
			int elemnt_index = pos % N;

			Chunk<value_type, allocator_type>* curr_chunk = first_chunk;
			while (chunk_index > 0) {
				curr_chunk = curr_chunk->next;
				chunk_index--;
			}
			return curr_chunk->list[elemnt_index];
		};

		reference operator[](difference_type pos) {
			int chunk_index = pos / N;
			int elemnt_index = pos % N;

			Chunk<value_type, allocator_type>* curr_chunk = first_chunk;
			while (chunk_index > 0) {
				curr_chunk = curr_chunk->next;
				chunk_index--;
			}
			return curr_chunk->list[elemnt_index];
		};

		const_reference operator[](difference_type pos) const {
			int chunk_index = pos / N;
			int elemnt_index = pos % N;

			Chunk<value_type, allocator_type>* curr_chunk = first_chunk;
			while (chunk_index > 0) {
				curr_chunk = curr_chunk->next;
				chunk_index--;
			}
			return curr_chunk->list[elemnt_index];
		};

		reference front() {
			if (list_size == 0)
				throw std::logic_error("Empty");

			return first_chunk->list[0];
		};

		const_reference front() const {
			if (list_size == 0)
				throw std::logic_error("Empty");

			return first_chunk->list[0];
		};

		reference back() {
			if (list_size == 0)
				throw std::logic_error("Empty");

			Chunk<value_type, allocator_type>* curr_chunk = last_chunk();

			return curr_chunk->list[curr_chunk->num_of_elements - 1];
		};

		const_reference back() const {
			if(list_size == 0)
				throw std::logic_error("Empty");

			Chunk<value_type, allocator_type>* curr_chunk = last_chunk();

			return curr_chunk->list[curr_chunk->num_of_elements - 1];
		};

		iterator begin() noexcept {
			return ChunkList_iterator<T>(this, 0, &at(0));
		};

		const_iterator begin() const noexcept {
			return ChunkList_const_iterator<T>(this, 0, &at(0));
		};

		const_iterator cbegin() const noexcept { return begin(); };

		iterator end() noexcept {
			return ChunkList_iterator<T>();
		};

		const_iterator end() const noexcept {
			return ChunkList_const_iterator<T>();
		};

		const_iterator cend() const noexcept { return end(); };

		bool empty() const noexcept { return list_size == 0; };

		size_type size() const noexcept { return list_size; };

		size_type max_size() const noexcept {
			int r = list_size % chunk_size;
			return (r == 0 ? list_size : list_size + N - r);
		};

		void shrink_to_fit() {
			Chunk<value_type, allocator_type>* curr_chunk = last_chunk();

			Chunk<value_type, allocator_type>* new_chunk = new Chunk<value_type, allocator_type>(N);
			for (size_type i = 0; i < curr_chunk->chunk_size; ++i) {
				new_chunk->list[i] = std::move<value_type>(curr_chunk->list[i]);
			}
			new_chunk->num_of_elements = curr_chunk->num_of_elements;

			delete[] curr_chunk->list;
			curr_chunk->list = new_chunk->list;
			curr_chunk->next = new_chunk;
		}

			Chunk<value_type, allocator_type>* cur = first_chunk;
			while (cur != nullptr) {
				Chunk<value_type, allocator_type>* tmp = cur;
				cur = cur->next;
				delete tmp;
			}
			list_size = 0;
			first_chunk = nullptr;
		};

		iterator insert(const_iterator pos, const T& value) {
			if (pos == cend()) {
				push_back(value);
				return end();
			}
			int index = pos.get_index();
			int i = 0;
			ChunkList_iterator<T> tmpPos = pos.constIteratorToIterator();
			Chunk<value_type, allocator_type>* curr_chunk = last_chunk();
			if (curr_chunk->chunk_size > curr_chunk->num_of_elements) {
				ChunkList_iterator<T> it = ChunkList_iterator<T>(
					this,
					list_size - 1,
					&curr_chunk->list[curr_chunk->chunk_size - 1]
				);
				for (; it >= tmpPos; it--, i++)
					at(list_size - i) = at(list_size - i - 1);
				list_size++;
			}
			else {
				curr_chunk->next = new Chunk<value_type, allocator_type>(N);
				ChunkList_iterator<T> it = ChunkList_iterator<T>(
					this,
					list_size - 1,
					&curr_chunk->list[curr_chunk->chunk_size - 1]
				);
				Chunk<value_type, allocator_type>* tmp = curr_chunk;
				curr_chunk = curr_chunk->next;
				curr_chunk->prev = tmp;
				list_size++;
				for (; it >= tmpPos; it--, i++)
					at(list_size - 1 - i) = at(list_size - i - 2);
			}
			at(index) = value;
			curr_chunk->num_of_elements++;
			return ChunkList_iterator<T>(this, index, &at(index));
		};

		iterator insert(const_iterator pos, T&& value) {
			if (pos == cend()) {
				push_back(value);
				return end();
			}
			int index = pos.get_index();
			int i = 0;
			ChunkList_iterator<T> tmpPos = pos.constIteratorToIterator();
			Chunk<value_type, allocator_type>* curr_chunk = last_chunk();
			if (curr_chunk->chunk_size > curr_chunk->num_of_elements) {
				ChunkList_iterator<T> it = ChunkList_iterator<T>(
					this,
					list_size - 1,
					&curr_chunk->list[curr_chunk->chunk_size - 1]
				);
				for (; it >= tmpPos; it--, i++)
					at(list_size - i) = at(list_size - i - 1);
				list_size++;
			}
			else {
				curr_chunk->next = new Chunk<value_type, allocator_type>(N);
				ChunkList_iterator<T> it = ChunkList_iterator<T>(
					this,
					list_size - 1,
					&curr_chunk->list[curr_chunk->chunk_size - 1]
				);
				Chunk<value_type, allocator_type>* tmp = curr_chunk;
				curr_chunk = curr_chunk->next;
				curr_chunk->prev = tmp;
				list_size++;
				for (; it >= tmpPos; it--, i++)
					at(list_size - 1 - i) = at(list_size - i - 2);
			}
			at(index) = std::move(value);
			curr_chunk->num_of_elements++;
			return ChunkList_iterator<T>(this, index, &at(index));
		};

		private:
		Chunk<value_type, allocator_type>* get_chunk_at_index(size_type index) const {
			int chunk_index = index / N;
			Chunk<value_type, allocator_type>* curr_chunk = first_chunk;
			while (chunk_index > 0) {
				curr_chunk = curr_chunk->next;
				chunk_index--;
			}
			return curr_chunk;
		}

		size_type get_start_index_of_chunk(Chunk<value_type, allocator_type>* chunk) const {
			size_type index = 0;
			Chunk<value_type, allocator_type>* curr_chunk = first_chunk;
			while (curr_chunk != chunk) {
				index += N;
				curr_chunk = curr_chunk->next;
			}
			return index;
		}

		public:
		iterator insert(const_iterator pos, size_type count, const T& value) {
			if (count == 0) {
				return ChunkList_iterator<T>(this, pos.get_index(), &at(pos.get_index()));
			}
			if (pos == cend()) {
				for (size_type i = 0; i < count; ++i) {
					push_back(value);
				}
				return ChunkList_iterator<T>(this, list_size - 1, &at(list_size - 1));
			}

			size_type index = pos.get_index();
			Chunk<value_type, allocator_type>* curr_chunk = get_chunk_at_index(index);
			size_type offset = index - get_start_index_of_chunk(curr_chunk);

			while (count > 0) {
				if (curr_chunk->num_of_elements < N - offset) {
					size_type num_to_copy = std::min(count, N - offset);
					std::copy_backward(curr_chunk->list + offset, curr_chunk->list + curr_chunk->num_of_elements,
						curr_chunk->list + curr_chunk->num_of_elements + num_to_copy);
					std::fill(curr_chunk->list + offset, curr_chunk->list + offset + num_to_copy, value);
					count -= num_to_copy;
					offset = 0;
					curr_chunk->num_of_elements = N;
				}
				else {
					Chunk<value_type, allocator_type>* new_chunk = new Chunk<value_type, allocator_type>();
					new_chunk->prev = curr_chunk;
					new_chunk->next = curr_chunk->next;
					if (curr_chunk->next != nullptr) {
						curr_chunk->next->prev = new_chunk;
					}
					curr_chunk->next = new_chunk;
					curr_chunk = new_chunk;
				}
			}

			list_size += count;
			return ChunkList_iterator<T>(this, index, &at(index));
		}

		Chunk<value_type, allocator_type>* insert_chunk_after(Chunk<value_type, allocator_type>* chunk) {
			Chunk<value_type, allocator_type>* new_chunk = new Chunk<value_type, allocator_type>();
			new_chunk->next = chunk->next;
			new_chunk->prev = chunk;
			if (chunk->next != nullptr) {
				chunk->next->prev = new_chunk;
			}
			chunk->next = new_chunk;
			return new_chunk;
		}
		template <class InputIt>
		iterator insert(const_iterator pos, InputIt first, InputIt last) {
			size_type index = pos.get_index();
			Chunk<value_type, allocator_type>* curr_chunk = get_chunk_at_index(index);
			size_type offset = index - get_start_index_of_chunk(curr_chunk);

			while (first != last) {
				if (offset == N) {
					curr_chunk = insert_chunk_after(curr_chunk);
					offset = 0;
				}
				if (curr_chunk->num_of_elements < N - offset) {
					size_type num_to_copy = std::min(static_cast<size_type>(std::distance(first, last)), N - offset);
					std::copy(first, first + num_to_copy, curr_chunk->list + offset);
					std::advance(first, num_to_copy);
					offset += num_to_copy;
					curr_chunk->num_of_elements += num_to_copy;
				}
			}

			list_size += std::distance(first, last);
			return ChunkList_iterator<T>(this, index, &at(index));
		}

		iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
			size_type index = pos.get_index();
			Chunk<value_type, allocator_type>* curr_chunk = get_chunk_at_index(index);
			size_type offset = index - get_start_index_of_chunk(curr_chunk);

			for (const auto& value : ilist) {
				if (offset == N) {
					curr_chunk = insert_chunk_after(curr_chunk);
					offset = 0;
				}
				if (curr_chunk->num_of_elements < N - offset) {
					curr_chunk->list[offset] = value;
					++offset;
					++curr_chunk->num_of_elements;
				}
			}

			list_size += ilist.size();
			return ChunkList_iterator<T>(this, index, &at(index));
		}

		template <class... Args>
		iterator emplace(const_iterator pos, Args&&... args) {
			return insert(pos, T(std::forward<Args>(args)...));
		}

		iterator erase(const_iterator pos) {
			auto index = pos.get_index();
			Chunk<value_type, allocator_type>* curr_chunk = last_chunk();
			curr_chunk->num_of_elements--;

			if (index + 1 == list_size) {
				list_size--;
				return end();
			}

			for (int i = index + 1; i < list_size; i++) {
				at(i - 1) = at(i);
			}

			if (curr_chunk->num_of_elements == 0) {
				curr_chunk = curr_chunk->prev;
				delete curr_chunk->next;
				curr_chunk->next = nullptr;
			}

			list_size--;
			return ChunkList_iterator<T>(this, index, &at(index));
		};

		iterator erase(const_iterator first, const_iterator last) {
			auto start_index = first.get_index();
			auto end_index = last.get_index();

			// Сдвигаем элементы влево, удаляя элементы в указанном диапазоне
			size_type shift = end_index - start_index + 1;
			for (size_type i = end_index + 1; i < list_size; ++i) {
				at(i - shift) = at(i);
			}

			// Обновляем переменную общее кол-во элементов
			list_size -= shift;

			// Проверяем и обновляем чанки при необходимости
			Chunk<value_type, allocator_type>* start_chunk = get_chunk_at_index(start_index);
			Chunk<value_type, allocator_type>* end_chunk = get_chunk_at_index(end_index);
			if (end_chunk != start_chunk) {
				start_chunk->next = end_chunk->next;
				if (end_chunk->next != nullptr) {
					end_chunk->next->prev = start_chunk;
				}
				else {
					end_chunk = start_chunk;
				}
			}
			end_chunk->num_of_elements -= shift - (end_index % N) - 1;

			// Возвращаем итератор, указывающий на первый элемент после удаленного диапазона
			return ChunkList_iterator<T>(this, start_index, &at(start_index));
		}

		void push_back(const T& value) {
			if (first_chunk == nullptr) {
				first_chunk = new Chunk<value_type, allocator_type>(N);
			}

			Chunk<value_type, allocator_type>* curr_chunk = last_chunk();
			if (curr_chunk->num_of_elements == N) {
				curr_chunk->next = new Chunk<value_type, allocator_type>(N);
				Chunk<value_type, allocator_type>* prev = curr_chunk;
				curr_chunk = curr_chunk->next;
				curr_chunk->prev = prev;
			}
			curr_chunk->list[curr_chunk->num_of_elements++] = value;
			list_size++;
		}

		void push_back(T&& value) {
			if (first_chunk == nullptr)
				first_chunk = new Chunk<value_type, allocator_type>(N);

			Chunk<value_type, allocator_type>* curr_chunk = last_chunk();
			if (curr_chunk->num_of_elements == N)
			{
				curr_chunk->next = new Chunk<value_type, allocator_type>(N);
				Chunk<value_type, allocator_type>* prev = curr_chunk;
				curr_chunk = curr_chunk->next;
				curr_chunk->prev = prev;
			}
			curr_chunk->list[curr_chunk->num_of_elements++] = std::move(value);
			list_size++;
		};

		template <class... Args>
		reference emplace_back(Args&&... args) {
			if (first_chunk == nullptr) {
				first_chunk = new Chunk<value_type, allocator_type>(N);
			}

			Chunk<value_type, allocator_type>* curr_chunk = last_chunk();
			if (curr_chunk->num_of_elements == N) {
				curr_chunk->next = new Chunk<value_type, allocator_type>(N);
				curr_chunk = curr_chunk->next;
			}

			curr_chunk->list[curr_chunk->num_of_elements++] = value_type(std::forward<Args>(args)...);;
			list_size++;
			return curr_chunk->list[curr_chunk->num_of_elements - 1];
		}

		void pop_back() {
			if (list_size == 0) {
				return;
			}

			list_size--;
			Chunk<value_type, allocator_type>* curr_chunk = last_chunk();
			curr_chunk->list[curr_chunk->num_of_elements - 1] = (value_type)nullptr;
			curr_chunk->num_of_elements--;

			if (curr_chunk->num_of_elements == 0 && first_chunk != curr_chunk) {
				Chunk<value_type, allocator_type>* prev_chunk = curr_chunk->prev;
				delete curr_chunk;
				prev_chunk->next = nullptr;
			}
		}

		void push_front(const T& value) {
			insert(cbegin(), value);
		};

		void push_front(T&& value) {
			insert(cbegin(), std::move(value));
		};

		template <class... Args>
		reference emplace_front(Args&&... args) {
			auto it = insert(cbegin(), value_type(std::forward<Args>(args)...));
			return *it;
		};

		void pop_front() {
			erase(cbegin());
		};

		void resize(size_type count) {
			if (count < 0)
				throw std::invalid_argument("Count can not be negative");

			Chunk<value_type, allocator_type>* curr_chunk = first_chunk;

			while (curr_chunk != nullptr) {
				curr_chunk->resize(count);
				curr_chunk = curr_chunk->next;
			}

			if (count < N) {
				list_size = 0;

				curr_chunk = first_chunk;
				while (curr_chunk != nullptr) {
					list_size += curr_chunk->num_of_elements;
					curr_chunk = curr_chunk->next;
				}
			}

			chunk_size = count;
		};

		void resize(size_type count, const value_type& value) {
			if (count < 0)
				throw std::invalid_argument("Count can not be negative");

			Chunk<value_type, allocator_type>* curr_chunk = first_chunk;

			while (curr_chunk != nullptr) {
				curr_chunk->resize(count, value);
				curr_chunk = curr_chunk->next;
			}

			if (count < N) {
				list_size = 0;

				curr_chunk = first_chunk;
				while (curr_chunk != nullptr) {
					list_size += curr_chunk->num_of_elements;
					curr_chunk = curr_chunk->next;
				}
			}

			chunk_size = count;
		};

		void swap(ChunkList<T, N, Allocator>& other) {
			std::swap(other.first_chunk, first_chunk);
			std::swap(other.list_size, list_size);
		}

		void print() {
			int chunk_num = 1;
			Chunk<value_type, allocator_type>* curr_chunk = first_chunk;
			while (true) {
				std::cout << "Chunk num: " << chunk_num << std::endl;
				for (value_type* el = curr_chunk->begin(); el != curr_chunk->end(); el++)
					std::cout << *el << "\t";
				std::cout << std::endl;

				chunk_num++;

				if (curr_chunk->next == nullptr) {
					break;
				}
				else {
					curr_chunk = curr_chunk->next;
				}
			}
		}

		friend bool operator==(const ChunkList<value_type, N, allocator_type>& lhs,
			const ChunkList<value_type, N, allocator_type>& rhs) {
			if (lhs.list_size != rhs.list_size)
				return false;

			for (int i = 0; i < lhs.list_size; i++)
				if (lhs.at(i) != rhs.at(i))
					return false;

			return true;
		};

		friend bool operator!=(const ChunkList<value_type, N, allocator_type>& lhs,
			const ChunkList<value_type, N, allocator_type>& rhs) {
			return !operator==(lhs, rhs);
		};

		friend bool operator>(const ChunkList<value_type, N, allocator_type>& lhs,
			const ChunkList<value_type, N, allocator_type>& rhs) {
			if (lhs.list_size != rhs.list_size) {
				return lhs.list_size > rhs.list_size;
			}
			else {
				for (int i = 0; i < lhs.list_size; i++)
					if (lhs.at(i) <= rhs.at(i))
						return false;

				return true;
			}
		};

		friend bool operator<(const ChunkList<value_type, N, allocator_type>& lhs,
			const ChunkList<value_type, N, allocator_type>& rhs) {
			return !operator>(lhs, rhs);
		};

		friend bool operator>=(const ChunkList<value_type, N, allocator_type>& lhs,
			const ChunkList<value_type, N, allocator_type>& rhs) {
			if (lhs.list_size < rhs.list_size) {
				return false;
			}
			else {
				for (int i = 0; i < lhs.list_size; i++)
					if (lhs.at(i) < rhs.at(i))
						return false;

				return true;
			}
		};

		friend bool operator<=(const ChunkList<value_type, N, allocator_type>& lhs,
			const ChunkList<value_type, N, allocator_type>& rhs) {
			return !operator>=(lhs, rhs);
		};

		// operator <=> will be handy
		std::strong_ordering operator<=>(const ChunkList& other) const {
			if (list_size < other.list_size) {
				return std::strong_ordering::less;
			}
			else if (list_size > other.list_size) {
				return std::strong_ordering::greater;
			}
			else {
				for (int i = 0; i < list_size; i++) {
					const auto& l = at(i);
					const auto& r = other.at(i);
					if (l < r) {
						return std::strong_ordering::less;
					}
					else if (l > r) {
						return std::strong_ordering::greater;
					}
				}
				return std::strong_ordering::equal;
			}
		}
	};

	template <class T, int N, class Alloc>
	void swap(ChunkList<T, N, Alloc>& lhs, ChunkList<T, N, Alloc>& rhs);

	template <class T, int N, class Alloc, class U>
	typename ChunkList<T, N, Alloc>::size_type erase(ChunkList<T, N, Alloc>& c, const U& value);

	template <class T, int N, class Alloc, class Pred>
	typename ChunkList<T, N, Alloc>::size_type erase_if(ChunkList<T, N, Alloc>& c, Pred pred);
}

