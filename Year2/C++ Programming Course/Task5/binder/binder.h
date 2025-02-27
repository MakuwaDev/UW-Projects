#ifndef BINDER_H
#define BINDER_H

#include <map>
#include <list>
#include <cstddef>
#include <memory>

namespace cxx {
	
	template <typename K, typename V>
	class binder {
		
		using notes_list = std::list<std::pair<const K&, V>>;
		using iter_map = std::map<K, typename notes_list::const_iterator>;
		
	private:
		static inline const notes_list empty_list = {};
		
		std::shared_ptr<notes_list> notes;
		std::shared_ptr<iter_map> iters;
		
		bool valid_non_const_reference;
		
		// Strong guarantee
		void copy_if_needed() {
			if (!notes || !notes.unique()) {
				auto new_list = notes ? std::make_shared<notes_list>(*notes) :
										std::make_shared<notes_list>();

				auto new_map = iters ? std::make_shared<iter_map>(*iters) :
										std::make_shared<iter_map>();

				iters = new_map;	// nothrow
				notes = new_list;	// nothrow
			}
		}
		
	public:
		binder() : valid_non_const_reference(false) {};
		
		binder(binder const &b) noexcept {
			if (b.valid_non_const_reference) {
				auto new_iters = std::make_shared<iter_map>();
				auto new_notes = std::make_shared<notes_list>();
				for( auto it = b.notes->rbegin(); it != b.notes->rend(); it++ )
				{
				    auto x = new_iters->insert( {it->first,new_notes->end()} );
				    new_notes->emplace_front( x.first->first, it->second );
				    x.first->second = new_notes->begin();
				}

				iters = new_iters;
				notes = new_notes;
			}
			else {
				iters = b.iters;
				notes = b.notes;
			}
			
			valid_non_const_reference = false;
		}
		
		constexpr binder(binder &&b) noexcept {
			iters = std::move(b.iters);
			notes = std::move(b.notes);
			valid_non_const_reference = std::move(b.valid_non_const_reference);
		}
		
		constexpr ~binder() = default;
		
		binder& operator=(binder b) {
			if (b.valid_non_const_reference) {
				auto new_iters = std::make_shared<iter_map>();
				auto new_notes = std::make_shared<notes_list>();
				for( auto it = b.notes->rbegin(); it != b.notes->rend(); it++ )
				{
				    auto x = new_iters->insert( {it->first,new_notes->end()} );
				    new_notes->emplace_front( x.first->first, it->second );
				    x.first->second = new_notes->begin();
				}
				
				iters = new_iters;
				notes = new_notes;
			}
			else {
				iters = b.iters;
				notes = b.notes;
			}
			
			valid_non_const_reference = false;
			return *this;
		}

		// Strong guarantee
		void insert_front(K const &k, V const &v) {
			// May throw, whatever
			if (iters && iters->count(k) == 1) {
				throw std::invalid_argument("Key already exists.");
			}
			
			std::shared_ptr<notes_list> new_notes;
			std::shared_ptr<iter_map> new_iters;

			// If allocation throws, nothing changed
			if (!notes) {
				new_notes = std::make_shared<notes_list>();
				new_iters = std::make_shared<iter_map>();
			}
			else if ( !notes.unique() ) {
				new_iters = std::make_shared<iter_map>();
				new_notes = std::make_shared<notes_list>();
				for( auto it = notes->rbegin(); it != notes->rend(); it++ )
				{
				    auto x = new_iters->insert( {it->first,new_notes->end()} );
				    new_notes->emplace_front( x.first->first, it->second );
				    x.first->second = new_notes->begin();
				}
			}
			else {
				new_notes = notes;
				new_iters = iters;
			}


			// If this throws, nothing changed
			auto x = new_iters->insert( {k,new_notes->end()} );
			new_notes->emplace_front( x.first->first, v );	// strong guarantee
			x.first->second = new_notes->begin();
			
			valid_non_const_reference = false;

			notes = new_notes;					// nothrow
			iters = new_iters;					// nothrow
		}

		// Strong guarantee
		void insert_after(K const &prev_k, K const &k, V const &v) {
			// These may throw but whatever
			if (iters && iters->count(k) == 1) {
				throw std::invalid_argument("Key already exists.");
			} else if (iters && iters->count(prev_k) == 0) {
				throw std::invalid_argument("There is no element in the binder under this key.");
			}

			std::shared_ptr<notes_list> new_notes;
			std::shared_ptr<iter_map> new_iters;

			// If allocation throws, nothing changed
			if (!notes) {
				new_notes = std::make_shared<notes_list>();
				new_iters = std::make_shared<iter_map>();
			}
			else if ( notes.use_count() > 1 ) {
				new_iters = std::make_shared<iter_map>();
				new_notes = std::make_shared<notes_list>();
				for( auto it = notes->rbegin(); it != notes->rend(); it++ )
				{
				    auto x = new_iters->insert( {it->first,new_notes->end()} );
				    new_notes->emplace_front( x.first->first, it->second );
				    x.first->second = new_notes->begin();
				}
			}
			else {
				new_notes = notes;
				new_iters = iters;
			}

			// If this throws, nothing changed
			auto pos = new_iters->find(prev_k)->second;				// strong guarantee
			++pos;

			auto x = new_iters->insert( {k,new_notes->end()} );
			auto it = new_notes->emplace( pos, x.first->first, v );	// strong guarantee
			x.first->second = it;
	
			valid_non_const_reference = false;

			notes = new_notes;	// nothrow
			iters = new_iters;	// nothrow
		}
		
		// Strong guarantee
		void remove() {
			// May throw, whatever
			if (!iters || iters->empty()) {
				throw std::invalid_argument("There is no element in the binder.");
			}
			
			std::shared_ptr<notes_list> new_notes = notes;
			std::shared_ptr<iter_map> new_iters = iters;

			if ( notes.use_count() > 2 )
			{
			    new_iters = std::make_shared<iter_map>();
			    new_notes = std::make_shared<notes_list>();
			    for( auto it = notes->rbegin(); it != notes->rend(); it++ )
			    {
				auto x = new_iters->insert( {it->first,new_notes->end()} );
				new_notes->emplace_front( x.first->first, it->second );
				x.first->second = new_notes->begin();
			    }
			}
			
			K k = new_notes->front().first;		// nothrow (list is not empty)
			new_iters->erase(k);				// nothrow
			new_notes->pop_front();				// nothrow
			
			iters = new_iters;				// nothrow
			notes = new_notes;				// nothrow

			valid_non_const_reference = false;
		}
		
		// Strong guarantee
		void remove(K const &k) {
			// May throw, whatever
			if (!iters) {
				throw std::invalid_argument("There is no element in the binder under this key.");
			}

			// May throw, whatever
			auto it = iters->find(k);		// strong guarantee
			if (it == iters->end()) {		// nothrow
				throw std::invalid_argument("There is no element in the binder under this key.");
			}
			
			std::shared_ptr<notes_list> new_notes = notes;
			std::shared_ptr<iter_map> new_iters = iters;

			if ( notes.use_count() > 2 )
			{
			    new_iters = std::make_shared<iter_map>();
			    new_notes = std::make_shared<notes_list>();
			    for( auto it = notes->rbegin(); it != notes->rend(); it++ )
			    {
				auto x = new_iters->insert( {it->first,new_notes->end()} );
				new_notes->emplace_front( x.first->first, it->second );
				x.first->second = new_notes->begin();
			    }

			}

			it = new_iters->find(k);

			new_notes->erase(it->second);		// nothrow
			new_iters->erase(it);				// nothrow
			
			iters = new_iters;			// nothrow
			notes = new_notes;			// nothrow

			valid_non_const_reference = false;
		}
		
		// Strong guarantee
		V & read(K const &k) {
			// May throw, whatever
			if (!iters) {
				throw std::invalid_argument("There is no element in the binder under this key.");
			}

			std::shared_ptr<notes_list> new_notes;
			std::shared_ptr<iter_map> new_iters;

			// If allocation throws, nothing changed
			if (!notes) {
				new_notes = std::make_shared<notes_list>();
				new_iters = std::make_shared<iter_map>();
			}
			else if ( notes.use_count() > 1 ) {
				new_iters = std::make_shared<iter_map>();
				new_notes = std::make_shared<notes_list>();
				for( auto it = notes->rbegin(); it != notes->rend(); it++ )
				{
				    auto x = new_iters->insert( {it->first,new_notes->end()} );
				    new_notes->emplace_front( x.first->first, it->second );
				    x.first->second = new_notes->begin();
				}
			}
			else {
				new_notes = notes;
				new_iters = iters;
			}

			// If this throws, nothing happened
			auto ret = new_iters->find(k);		// strong guarantee
			if (ret == new_iters->end()) {		//nothrow
				throw std::invalid_argument("There is no element in the binder under this key.");
			}
			
			notes = new_notes;	// nothrow
			iters = new_iters;	// nothrow
			return const_cast<V&>(ret->second->second);
		}
		
		// Strong guarantee
		V const & read(K const &k) const {
			// May throw, whatever
			if (!iters) {
				throw std::invalid_argument("There is no element in the binder under this key.");
			}

			auto ret = iters->find(k);		// strong guarantee
			if (ret == iters->end()) {		// nothrow
				throw std::invalid_argument("There is no element in the binder under this key.");
			}
			
			return ret->second->second;
		}
		
		std::size_t size() const noexcept {
			return notes ? notes->size() : 0;
		}
		
		void clear() noexcept {
			notes = nullptr;
			iters = nullptr;
			valid_non_const_reference = false;
		}
		
		//iterator
		class const_iterator {
		private:
			using base_iterator = typename notes_list::const_iterator;
			base_iterator it;

		public:
			using difference_type   = std::ptrdiff_t;
			using value_type        = V;
			using pointer           = V const*;
			using reference         = V const&;
			using iterator_category = std::forward_iterator_tag;
			
			const_iterator() = default;

			explicit const_iterator(base_iterator iter) noexcept : it(iter) {}

			const_iterator(const_iterator const &other) = default;
			const_iterator(const_iterator &&other) = default;
			
			~const_iterator() = default;
			
			reference operator*() const noexcept {
				return (*it).second;
			}
			
			pointer operator->() const noexcept {
				return &((*it).second);
			}

			const_iterator &operator=(const_iterator const &other) = default;
			const_iterator &operator=(const_iterator &&other) = default;
			
			bool operator==(const_iterator const &other) const noexcept {
				return it == other.it;	// nothrow
			}
			
			// prefix
			const_iterator &operator++() noexcept {
				++it;
				return *this;
			}
			
			// postfix
			const_iterator operator++(int) noexcept {
				const_iterator temp = *this;
				++it;
				return temp;
			}
			
		}; // const_iterator
		
		const_iterator cbegin() const noexcept {
			return notes ? const_iterator(notes->cbegin()) : const_iterator(empty_list.cbegin());
		}
		
		const_iterator cend() const noexcept {
			return notes ? const_iterator(notes->cend()) : const_iterator(empty_list.cend());
		}
		
	}; // class binder
} // namespace cxx

#endif
