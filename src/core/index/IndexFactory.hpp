#ifndef IndexFactory_hpp
#define IndexFactory_hpp

#include "Index.hpp"
#include "btree/BPlusTreeIndex.hpp"

class IndexFactory {
public:
	IndexFactory(IndexFactory const &) = delete;
	IndexFactory & operator= (IndexFactory const &) = delete;

	static IndexFactory & GetInstance();

	template<typename ... Args>
	std::unique_ptr<IIndex> CreateIndex(Args && ... args);

private:
	IndexFactory() = default;
	~IndexFactory() = default;
};


IndexFactory & IndexFactory ::GetInstance() {
	static IndexFactory factory;
	return factory;
}

template<typename ... Args>
std::unique_ptr<IIndex> IndexFactory::CreateIndex(IndexType type, Args && ... args) {
	switch (type) {
		case IndexType::BTREE:
			return std::make_unique<BPlusTreeIndex>(std::forward<Args>(args)...);
		case IndexType::HASH:
			throw std::runtime_error("Hash index not implemented yet :(");
		default:
			throw std::runtime_error("Unknown index type.");
	}
}

#endif // IndexFactory_hpp
