/**
 * 实现list
 * 
 * 
 * @author YC奕晨 
 * */ 

 #ifndef LIST_HPP_
 #define LIST_HPP_
 
 #include <memory>
 #include <stdexcept>
 #include <iterator>
 #include <type_traits>
 #include <iostream>
 
 namespace ycstl {
 
 template<typename T>
 struct ListNode {
     ListNode* pre_;
     ListNode* next_;
     T value_;
 };
 
 template<typename T, typename Allocator = std::allocator<T>>
 class list {
     class Iterator;                 // iterator
     class ConstIterator;            // const iterator
 
     //重新绑定分配器，可以分配ListNode<T>大小内存
     using NodePtr = ListNode<T>*;
     using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<ListNode<T>>;
 
 public:
     using value_type             = T;
     using allocator_type         = Allocator;
     using pointer                = typename std::allocator_traits<Allocator>::pointer;
     using const_pointer          = typename std::allocator_traits<Allocator>::const_pointer;
     using reference              = value_type&;
     using const_reference        = const value_type&;
     using size_type              = std::size_t;
     using difference_type        = std::ptrdiff_t;
     using iterator               = Iterator;
     using const_iterator         = ConstIterator;
     using reverse_iterator       = std::reverse_iterator<iterator>;
     using const_reverse_iterator = std::reverse_iterator<const_iterator>;
 
     list() noexcept : list(Allocator()) {}
 
     explicit list(const Allocator& alloc) noexcept :  size_(0), alloc_(alloc) {
         NodePtr end_node = proxy_construct(alloc_);
         begin_.cur_ = end_.cur_ = end_node;
     }
 
     explicit list(size_type n, const Allocator& alloc = Allocator()) noexcept : size_(n), alloc_(alloc) {
         proxy_construct(n,alloc);
     }
 
     explicit list(size_type n, const T& value, const Allocator& alloc = Allocator()) noexcept : size_(n), alloc_(alloc) {
         proxy_construct(n,value,alloc);
     }
 
     template<typename InputIter, typename = std::void_t<
         decltype(*std::declval<InputIter>()), 
         decltype(++std::declval<InputIter&>())   
     >>
     list(InputIter first, InputIter last, const Allocator& alloc = Allocator())  noexcept : alloc_(alloc) {
         NodePtr end_node = proxy_construct(alloc_);
         begin_ = end_ = Iterator(end_node);
         std::size_t cur_size = 0;
         for (InputIter it = first; it != last; ++it) {
             ++cur_size;
             NodePtr cur_node = proxy_construct(alloc);
             cur_node->value_ = *it;
             cur_node->next_ = end_.GetNodePtr();
             if (begin_ == end_) {
                 begin_ = Iterator(cur_node);
                 end_.GetNodePtr()->pre_ = cur_node;
             } else {
                 cur_node->pre_ = end_.GetNodePtr()->pre_;
                 end_.GetNodePtr()->pre_->next_ = cur_node;
                 end_.GetNodePtr()->pre_ = cur_node;
             }
         }
         size_ = cur_size;
     }
 
     list(const list& x) noexcept : list(x.begin(), x.end(), x.alloc_) {}
 
     list(list&& x) noexcept : begin_(std::move(x.begin_)), end_(std::move(x.end_)), 
             size_(x.size_), alloc_(std::move(x.alloc_)) {
         x.begin_.cur_ = x.end_.cur_ = nullptr;
         x.size_ = 0;
     }
 
     list(std::initializer_list<T> il, const Allocator& alloc = Allocator()) noexcept : list(il.begin(), il.end(), alloc) {}
 
     ~list() {
         clean();
     }
 
     list& operator=(const list& other) {
         clean();
         NodePtr end_node = proxy_construct(alloc_);
         begin_ = end_ = Iterator(end_node);
         std::size_t cur_size = 0;
         for (Iterator it = other.begin_; it != other.end_; ++it) {
             ++cur_size;
             NodePtr cur_node = proxy_construct(alloc_);
             cur_node->value_ = *it;
             cur_node->next_ = end_.GetNodePtr();
             if (begin_ == end_) {
                 begin_ = Iterator(cur_node);
                 end_.GetNodePtr()->pre_ = cur_node;
             } else {
                 cur_node->pre_ = end_.GetNodePtr()->pre_;
                 end_.GetNodePtr()->pre_->next_ = cur_node;
                 end_.GetNodePtr()->pre_ = cur_node;
             }
         }
         size_ = cur_size;
         return *this;
     }
 
     list& operator=(list&& other) noexcept {
         clean();
         NodePtr end_node = proxy_construct(alloc_);
         begin_ = end_ = Iterator(end_node);
         std::size_t cur_size = 0;
         for (Iterator it = other.begin_; it != other.end_; ++it) {
             ++cur_size;
             NodePtr cur_node = proxy_construct(alloc_);
             cur_node->value_ = *it;
             cur_node->next_ = end_.GetNodePtr();
             if (begin_ == end_) {
                 begin_ = Iterator(cur_node);
                 end_.GetNodePtr()->pre_ = cur_node;
             } else {
                 cur_node->pre_ = end_.GetNodePtr()->pre_;
                 end_.GetNodePtr()->pre_->next_ = cur_node;
                 end_.GetNodePtr()->pre_ = cur_node;
             }
         }
         size_ = cur_size;
         other.clean();
 
         return *this;
     }
 
     list& operator=(std::initializer_list<T> ilist) {
         clean();
         *this = list(ilist);
         return *this;
     }
 
     void assign(std::size_t n, const T& t) {
         *this = list(n,t);
     }
 
     void assign(std::initializer_list<T> l) {
         *this = list(l);
     }
 
     iterator begin() noexcept {
         return begin_;
     }
 
     const_iterator begin() const noexcept {
         return begin_;
     }
 
     iterator end() noexcept {
         return end_;
     }
 
     const_iterator end() const{
         return end_;
     }
 
     reverse_iterator rbegin() noexcept {
         return std::reverse_iterator<iterator>(end_);
     }
 
     reverse_iterator rbegin() const noexcept {
         return std::reverse_iterator<iterator>(end_);
     }
 
     const_reverse_iterator rend() noexcept {
         return std::reverse_iterator<iterator>(begin_);
     }
 
     const_reverse_iterator rend() const noexcept {
         return std::reverse_iterator<iterator>(begin_);
     }
 
     const_iterator cbegin() const noexcept {
         return begin_;
     }
 
     const_iterator cend() const noexcept {
         return end_;
     }
 
     const_reverse_iterator& crbegin() const noexcept {
         return std::reverse_iterator<iterator>(begin_);
     }
 
     const_reverse_iterator& crend() const noexcept {
         return std::reverse_iterator<iterator>(end_);
     }
 
     bool empty() const noexcept {
         return 0 == size_;
     }
 
     size_type size() const noexcept {
         return size_;
     }
 
     void resize(size_type sz) {
         if (sz == size_) {
             return;
         }
         while (size_ > sz) {
             clean_last();
         }
         while (size_ < sz) {
             append_back(nullptr);
         } 
     } 
 
     void resize(size_type sz, const T& c) {
         if (sz == size_) {
             return;
         }
         while (size_ > sz) {
             clean_last();
         }
         while (size_ < sz) {
             append_back(&c);
         } 
     }
 
     reference front() {
         return *begin_;
     }
 
     const_reference front() const {
         return *begin_;
     }
 
     reference back() {
         return end_.cur_->pre_->value_;
     }
 
     const_reference back() const {
         return end_.cur_->pre_->value_;
     }
 
     template<typename... Args>
     reference emplace_front(Args&&... args) {
         NodeAllocator node_alloc{alloc_};
         NodePtr node = node_alloc.allocate(1);
         std::construct_at(&node->value_, std::forward<Args>(args)...);
         node->next_ = begin_.cur_;
         node->pre_ = begin_.cur_->pre_;
         begin_.cur_->pre_ = node;
         begin_.cur_ = node;
         ++size_;
         return node->value_;
     }
 
     template<typename... Args>
     reference emplace_back(Args&&... args) {
         NodeAllocator node_alloc{alloc_};
         NodePtr node = node_alloc.allocate(1);
         std::construct_at(&node->value_, std::forward<Args>(args)...);
         node->next_ = end_.cur_;
         node->pre_ = end_.cur_->pre_;
         if (node->pre_ != nullptr) {
             node->pre_->next_ = node;
         }
         node->next_->pre_ = node;
         ++size_;
         if (1 == size_) {
             begin_.cur_ = node;
         }
         return node->value_;
     }
 
     void push_front(const T& x) {
         emplace_front(x);
     }
 
     void push_front(T&& x) {
         emplace_front(std::move(x));
     }
 
     void pop_front() {
         clear_first();
     }
 
     void push_back(const T& x) {
         emplace_back(x);
     }
 
     void push_back(T&& x) {
         emplace_back(std::move(x));
     }
 
     void pop_back() {
         clean_last();
     }
 
     template<typename... Args>
     iterator emplace(const_iterator position, Args&&... args) {
         // 插入到指定迭代器之前的位置
         NodeAllocator node_alloc{alloc_};
         NodePtr node = node_alloc.allocate(1);
         std::construct_at(&node->value_, std::forward<Args>(args)...);
         node->next_ = position.cur_;
         node->pre_ = position.cur_->pre_;
         if (node->pre_ != nullptr) {
             node->pre_->next_ = node;
         }
         node->next_->pre_ = node;
         if (node->pre_ == nullptr) {
             begin_.cur_ = node;
         }
         ++size_;
         return Iterator(node);
     }
 
     iterator insert(const_iterator position, const T& x) {
         return emplace(position, x);
     }
 
     iterator insert(const_iterator position, T&& x) {
         return emplace(position, std::move(x));
     }
 
     iterator insert(const_iterator position, size_type n, const T& x) {
         iterator first_iterator = emplace(position, x);
         --n;
         while (0 != n) {
             emplace(position,x);
             --n;
         }
         return first_iterator;
     }
 
     template<typename InputIter, typename = std::void_t<
         decltype(*std::declval<InputIter>()),
         decltype(++std::declval<InputIter&>())
     >>
     iterator insert(const_iterator position, InputIter first, InputIter last) {
         InputIter it = first;
         iterator first_iterator = emplace(position, *it);
         ++it;
         while(it != last) {
             emplace(position, *it);
             ++it;
         }
         return first_iterator;
     }
 
     iterator insert(const_iterator position, std::initializer_list<T> il) {
         auto it = il.begin();
         auto end = il.end();
         iterator first_iterator = emplace(position, *it);
         ++it;
         while (it != end) {
             emplace(position, *it);
             ++it;
         }
         return first_iterator;
     }
 
     iterator erase(const_iterator position) {
         NodeAllocator node_alloc{alloc_};
         Iterator follow_iterator(position.cur_->next_);
         NodePtr delete_node = position.cur_;
         if (delete_node->pre_ != nullptr) {
             delete_node->pre_->next_ = delete_node->next_;
         }
         delete_node->next_->pre_ = delete_node->pre_;
         if (delete_node == begin_.cur_) {
             begin_.cur_ = delete_node->next_;
         }
         std::destroy_at(delete_node);
         node_alloc.deallocate(delete_node,1);
         --size_;
         return follow_iterator;
     }
 
     iterator erase(const_iterator position, const_iterator last) {
         while (last.cur_->pre_ != position.cur_) {
             erase(Iterator(last.cur_->pre_));
         }
         erase(Iterator(last.cur_->pre_));
         return Iterator(last.cur_);
     }
 
     void swap(list& l) noexcept(std::allocator_traits<Allocator>::is_always_equal::value) {
         auto tmp_begin = l.begin_.cur_;
         auto tmp_end = l.end_.cur_;
         auto tmp_size = l.size_;
 
         l.begin_.cur_ = begin_.cur_;
         l.end_.cur_ = end_.cur_;
         l.size_ = size_;
 
         begin_.cur_ = tmp_begin;
         end_.cur_ = tmp_end;
         size_ = tmp_size;
     }
 
     void clear() noexcept {
         erase(begin_,end_);
     }
 
     void splice(const_iterator position, list& x) {
         while (!x.empty()) {
             splice(position, x, x.begin());
         }
     }
 
     void splice(const_iterator position, list&& x) {
         while (!x.empty()) {
             splice(position, std::move(x), x.begin());
         }
     }
 
     void splice (const_iterator position, list& x, const_iterator i) {
         NodePtr node = i.cur_;
         if (nullptr != node->pre_) {
             node->pre_->next_ = node->next_;
         }
         node->next_->pre_ = node->pre_;
         if (x.begin_.cur_ == node) {
             x.begin_.cur_ = node->next_;
         }
         --x.size_;
         node->pre_ = position.cur_->pre_;
         node->next_ = position.cur_;
         if (nullptr != node->pre_) {
             node->pre_->next_ = node;
         }
         node->next_->pre_ = node;
         if (begin_.cur_ == position.cur_) {
             begin_.cur_ = node;
         }
         ++size_;
     }
 
     void splice(const_iterator position, list&& x, const_iterator i) {
         NodePtr node = i.cur_;
         if (nullptr != node->pre_) {
             node->pre_->next_ = node->next_;
         }
         node->next_->pre_ = node->pre_;
         if (x.begin_.cur_ == node) {
             x.begin_.cur_ = node->next_;
         }
         --x.size_;
         node->pre_ = position.cur_->pre_;
         node->next_ = position.cur_;
         if (nullptr != node->pre_) {
             node->pre_->next_ = node;
         }
         node->next_->pre_ = node;
         if (begin_.cur_ == position.cur_) {
             begin_.cur_ = node;
         }
         ++size_;
     }
 
     void splice(const_iterator position, list& x, const_iterator first, const_iterator last) {
         const_iterator next_it = first;
         const_iterator cur_it = next_it++;
         while (next_it != last) {
             splice(position, x, cur_it);
             cur_it = next_it++;
         }
         splice(position,x,cur_it);
     }
 
     void splice(const_iterator position, list&& x, const_iterator first, const_iterator last) {
         const_iterator next_it = first;
         const_iterator cur_it = next_it++;
         while (next_it != last) {
             splice(position, std::move(x), cur_it);
             cur_it = next_it++;
         }
         splice(position,x,cur_it);
     }
 
     size_type remove(const T& value) {
         auto next_it = begin_;
         auto cur_it = next_it++;
         while(next_it != end_) {
             if (cur_it.cur_->value_ == value) {    
                 erase(cur_it);
             }
             cur_it = next_it++;
         }
         if (cur_it.cur_->value_ == value) {
             erase(cur_it);
         }
         return size_;
     }
 
     template <typename Predicate, typename = std::enable_if_t<
         std::is_invocable_r_v<bool, Predicate, T>       // 检查可调用对象的type traits工具
     >>
     auto remove_if(Predicate pred) {
         auto next_it = begin_;
         auto cur_it = next_it++;
         while(next_it != end_) {
             if (pred(cur_it.cur_->value_)) {    
                 erase(cur_it);
             }
             cur_it = next_it++;
         }
         if (pred(cur_it.cur_->value_)) {
             erase(cur_it);
         }
         return size_;
     }
 
     size_type unique() {
         auto next_it = begin_;
         ++next_it;
         auto cur_it = next_it++;
         while(next_it != end_) {
             if (cur_it.cur_->value_ == cur_it.cur_->pre_->value_) {    
                 erase(cur_it);
             }
             cur_it = next_it++;
         }
         if (cur_it.cur_->value_ == cur_it.cur_->pre_->value_) {
             erase(cur_it);
         }
         return size_;
     }
 
     template<typename BinaryPredicate, typename = std::enable_if_t<
         std::is_invocable_r_v<bool, BinaryPredicate, T, T>
     >>
     size_type unique(BinaryPredicate pred) {
         auto next_it = begin_;
         ++next_it;
         auto cur_it = next_it++;
         while(next_it != end_) {
             if (pred(cur_it.cur_->pre_->value_, cur_it.cur_->value_)) {    
                 erase(cur_it);
             }
             cur_it = next_it++;
         }
         if (pred(cur_it.cur_->pre_->value_, cur_it.cur_->value_)) {
             erase(cur_it);
         }
         return size_;
     }
 
 
     // 使用归并前应当保证链表有序，否则是ub. to be continue......
     // void merge(list& x) {
 
     // }
 
     // void merge(list&& x) {
 
     // }
 
     // template<typename Compare, typename = std::enable_if_t<
     //     std::is_invocable_r_v<bool, Compare, T, T>
     // >>
     // void merge(list& x, Compare comp) {
 
     // }
 
     // template<typename Compare, typename = std::enable_if_t<
     //     std::is_invocable_r_v<bool, Compare, T, T>
     // >>
     // void merge(list&& x, Compare comp) {
 
     // }
 
     
 private:
     NodePtr proxy_construct(const Allocator& alloc) noexcept {
         NodeAllocator node_alloc{alloc};
         NodePtr node = node_alloc.allocate(1);
         std::construct_at(&node->value_);
         node->next_ = nullptr;
         node->pre_ = nullptr;
         return node;
     }
 
     void proxy_construct(size_type n, const Allocator& alloc)  noexcept {
         NodeAllocator node_alloc{alloc};
         NodePtr node = node_alloc.allocate(1);
         std::construct_at(&node->value_);
         node->pre_ = nullptr;
         node->next_ = nullptr;
         begin_ = Iterator(node);
         for (size_t i = 1; i < n; ++i) {
             NodePtr new_node = node_alloc.allocate(1);
             std::construct_at(&new_node->value_);
             new_node->pre_ = node;
             new_node->next_ = node->next_;
             node->next_ = new_node;
             node = new_node;
         }
         NodePtr end_node = node_alloc.allocate(1);
         std::construct_at(&end_node->value_);
         end_node->pre_ = node;
         end_node->next_ = node->next_;
         node->next_ = end_node;
         end_ = Iterator(end_node);
     }
 
     void proxy_construct(size_type n, const T& value, const Allocator& alloc)  noexcept {
         NodeAllocator node_alloc{alloc};
         NodePtr node = node_alloc.allocate(1);
         std::construct_at(&node->value_,value);
         node->pre_ = nullptr;
         node->next_ = nullptr;
         begin_ = Iterator(node);
         for (size_t i = 1; i < n; ++i) {
             NodePtr new_node = node_alloc.allocate(1);
             std::construct_at(&new_node->value_, value);
             new_node->pre_ = node;
             new_node->next_ = node->next_;
             node->next_ = new_node;
             node = new_node;
         }
         NodePtr end_node = node_alloc.allocate(1);
         std::construct_at(&end_node->value_);
         end_node->pre_ = node;
         end_node->next_ = node->next_;
         node->next_ = end_node;
         end_ = Iterator(end_node);
     }
 
     void clean() {
         if (begin_.cur_ == nullptr && end_.cur_ == nullptr) {
             return;
         }
         NodeAllocator node_alloc{alloc_};
         NodePtr delete_node = begin_.cur_;
         while (delete_node != end_.cur_) {
             NodePtr next_node = delete_node->next_;
             std::destroy_at(&delete_node->value_);
             node_alloc.deallocate(delete_node, 1);
             delete_node = next_node;
         }
         NodePtr end_node = end_.cur_;
         std::destroy_at(end_node);
         node_alloc.deallocate(end_node,1);
         begin_.cur_ = end_.cur_ = nullptr;
         size_ = 0;
     }
 
     void clean_last() {     // 删除最后一个节点
         if (0 == size_) {
             return;
         }
         if (1 == size_) {
             begin_.cur_ = end_.cur_;
         }
         NodeAllocator node_alloc{alloc_};
         NodePtr delete_node = end_.cur_->pre_;
         if (delete_node->pre_ != nullptr) {
             delete_node->pre_->next_ = delete_node->next_;
         }
         delete_node->next_->pre_ = delete_node->pre_;
         std::destroy_at(delete_node);
         node_alloc.deallocate(delete_node,1);
         --size_;
     }   
 
     void clear_first() {
         if (0 == size_) {
             return;
         }
         NodeAllocator node_alloc{alloc_};
         NodePtr delete_node = begin_.cur_;
         begin_.cur_ = delete_node->next_;
         begin_.cur_->pre_ = delete_node->pre_;
         std::destroy_at(delete_node);
         node_alloc.deallocate(delete_node,1);
         --size_;
     }
 
 
     void append_back(const T* t) {
         NodePtr end_node = proxy_construct(alloc_);
         end_node->next_ = end_.cur_;
         end_node->pre_ = end_.cur_->pre_;
         if (end_node->pre_ != nullptr) {
             end_node->pre_->next_ = end_node;
         }
 
         end_node->next_->pre_ = end_node;
         if (nullptr != t) {
             end_node->value_ = *t;
         }
         ++size_;
     }
 
 private:
     iterator begin_;
     iterator end_;
     size_type size_;
     Allocator alloc_;
 
     class Iterator {
     public:
         using iterator_category = std::bidirectional_iterator_tag; // 双向迭代器
         using value_type = T;
         using difference_type = std::ptrdiff_t;
         using pointer = T*;
         using reference = T&;
         
         Iterator(){}
 
         explicit Iterator(ListNode<T>* cur) {
             cur_ = cur;
         }
 
         Iterator(const Iterator& it) {
             cur_ = it.cur_;
         }
 
         Iterator(Iterator&& it) {
             cur_ = it.cur_;
             it.cur_ = nullptr;
         }
 
         Iterator& operator=(const Iterator& it) {
             if (&it == this) {
                 return *this;
             }
             cur_ = it.cur_;
             return *this;
         }
 
         Iterator& operator=(Iterator&& it) {
             if (&it == this) {
                 return *this;
             }
             cur_ = it.cur_;
             it.cur_ = nullptr;
             return *this;
         } 
 
         reference operator*() {
             return cur_->value_;
         }
 
         pointer operator->() {
             return &(cur_->value_);
         }
 
         Iterator& operator++() {
             cur_ = cur_->next_;
             return *this;
         }
 
         Iterator operator++(int) {
             Iterator it(cur_);
             cur_ = cur_->next_;
             return it;
         }
 
         Iterator& operator--() {
             cur_ = cur_->pre_;
             return *this;
         }
 
         Iterator operator--(int) {
             Iterator it(cur_);
             cur_ = cur_->pre_;
             return it;
         }
 
         NodePtr GetNodePtr(){
             return cur_;
         }
 
         bool operator==(const Iterator& it) {
             return cur_ == it.cur_;
         }
 
         bool operator!=(const Iterator& it) {
             return cur_ != it.cur_;
         }
 
     private:
         NodePtr cur_;
         friend class list;
     };
 
     // 需要一个const_iterator来禁止修改数据
     class ConstIterator {
     public:
         using iterator_category = std::bidirectional_iterator_tag;
         using value_type = T;
         using difference_type = std::ptrdiff_t;
         using pointer = const T*;
         using reference = const T&;
     
         ConstIterator() : cur_(nullptr) {}
     
         explicit ConstIterator(ListNode<T>* cur) : cur_(cur) {}
     
         ConstIterator(const ConstIterator& it) : cur_(it.cur_) {}
     
         ConstIterator(const Iterator& it) : cur_(it.cur_) {} // 允许从 Iterator 转换成 ConstIterator
     
         reference operator*() const {
             return cur_->value_;
         }
     
         pointer operator->() const {
             return &(cur_->value_);
         }
     
         ConstIterator& operator++() {
             cur_ = cur_->next_;
             return *this;
         }
     
         ConstIterator operator++(int) {
             ConstIterator temp(*this);
             cur_ = cur_->next_;
             return temp;
         }
     
         ConstIterator& operator--() {
             cur_ = cur_->pre_;
             return *this;
         }
     
         ConstIterator operator--(int) {
             ConstIterator temp(*this);
             cur_ = cur_->pre_;
             return temp;
         }
     
         bool operator==(const ConstIterator& it) const {
             return cur_ == it.cur_;
         }
     
         bool operator!=(const ConstIterator& it) const {
             return cur_ != it.cur_;
         }
     
     private:
         ListNode<T>* cur_;
         friend class list;
     };
 
 };
 
 template <typename T>
 std::ostream& operator<<(std::ostream& os, const ycstl::list<T>& l) {
     os << "{";
     for (auto it = l.begin(); it != l.end(); ++it) {
         os << *it;
         os << "-> ";
     }
     os << "end}";
     return os;
 }
 
 }
 
 #endif