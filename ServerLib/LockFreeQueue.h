#pragma once


template<typename T>
class LockFreeQueue{

	struct Node;

	struct CountedNodePtr {
		int32 externalCount;	// 참조권
		Node* ptr = nullptr;
	};
	
	struct NodeCounter {
		uint32 internalCount : 30;	// 내부 참조 카운트(노드 삭제 조건 확인)
		uint32 externalCountRemaining : 2; // Push & Pop 다중 참조권 관련
	};

	struct Node {

		Node() {

			NodeCounter newCount;
			newCount.internalCount = 0;
			newCount.externalCountRemaining = 2;

			count.store(newCount);

			next.ptr = nullptr;
			next.externalCount = 0;
		}

		void ReleaseRef() {
			NodeCounter oldCounter = count.load();

			while (true)
			{
				NodeCounter newCounter = oldCounter;
				newCounter.internalCount--;

				if (count.compare_exchange_strong(oldCounter, newCounter))
				{
					if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
						delete this;

					break;
				}
			}
		}

		atomic<T*> data;
		atomic<NodeCounter> count;
		CountedNodePtr next;
	};

public:
	LockFreeQueue(){
		CountedNodePtr initHead;
		initHead.ptr = new Node();
		initHead.externalCount = 1;
		
		_head.store(initHead);
		_head.store(initHead);
	}

	void Push(const T& value) {

		unique_ptr<T> newData = make_unique<T>(value);
		
		CountedNodePtr dummy;
		dummy.ptr = new Node;
		dummy.externalCount = 1;

		CountedNodePtr oldTail = _tail.load();

		while (true) {
			// 참조권 획득(externalCount를 현시점 기준 +1 한 애가 이기
			IncreaseExternalCount(_tail, oldTail);

			// 소유권 획득 (data를 먼저 교환한 애가 이김)
			T* oldData = nullptr;
			if (oldTail.ptr->data.compare_exchange_strong(oldData, newData.get())) {
				oldTail.ptr->next = dummy;
				oldTail = _tail.exchange(dummy);
				FreeExternalCount(oldTail);
				newData.release();
				break;
			}

			// 소유권 경쟁 패배
			oldTail.ptr->ReleaseRef();
		}
	}

	T* TryPop() {

		CountedNodePtr oldHead = _head.load();

		while (true)
		{
			IncreaseExternalCount(_head, oldHead);

			Node* ptr = oldHead.ptr;
			if (ptr == _tail.load().ptr)
			{
				ptr->ReleaseRef();
				return shared_ptr<T>();
			}

			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				T* res = ptr->data.load();
				FreeExternalCount(oldHead);
				return shared_ptr<T>(res);
			}

			ptr->ReleaseRef();
		}
	}

private:
	static void IncreaseExternalCount(atomic<CountedNodePtr>& counter, CountedNodePtr& oldCounter) {

		while (true) {
			CountedNodePtr newCounter = oldCounter;
			newCounter.externalCount++;
			
			if (counter.compare_exchange_strong(oldCounter, newCounter)) {
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}
	static void FreeExternalCount(CountedNodePtr& oldNodePtr) {
		Node* ptr = oldNodePtr.ptr;
		const int32 countIncrease = oldNodePtr.externalCount - 2;

		NodeCounter oldCounter = ptr->count.load();

		while (true)
		{
			NodeCounter newCounter = oldCounter;
			newCounter.externalCountRemaining--; 
			newCounter.internalCount += countIncrease;

			if (ptr->count.compare_exchange_strong(oldCounter, newCounter))
			{
				if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
					delete ptr;

				break;
			}
		}
	}

private:
	atomic<CountedNodePtr> _head = nullptr;
	atomic<CountedNodePtr> _tail = nullptr;

};

