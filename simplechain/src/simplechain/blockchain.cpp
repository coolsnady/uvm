#include <simplechain/blockchain.h>
#include <simplechain/operations.h>
#include <simplechain/simplechain_uvm_api.h>
#include <iostream>

namespace simplechain {
	blockchain::blockchain() {
		uvm::lua::api::global_uvm_chain_api = new simplechain::SimpleChainUvmChainApi();

		asset core_asset;
		core_asset.asset_id = 0;
		core_asset.precision = 5;
		core_asset.symbol = "COIN";
		assets.push_back(core_asset);

		block genesis_block;
		genesis_block.block_number = 0;
		genesis_block.block_time = fc::time_point(fc::microseconds(1536033055382L));
		blocks.push_back(genesis_block);
	}

	void blockchain::evaluate_transaction(std::shared_ptr<transaction> tx) {
		try {
			for (const auto& op : tx->operations) {
				auto evaluator_instance = get_operation_evaluator(tx.get(), op);
				auto op_result = evaluator_instance->evaluate(op);
			}
		}
		catch (const std::exception& e) {
			throw e;
		}
	}
	void blockchain::apply_transaction(std::shared_ptr<transaction> tx) {
		try {
			for (const auto& op : tx->operations) {
				auto evaluator_instance = get_operation_evaluator(tx.get(), op);
				auto op_result = evaluator_instance->apply(op);
			}
			// TODO: save evaluate state
		}
		catch (const std::exception& e) {
			throw e;
		}
	}

	block blockchain::latest_block() const {
		assert( ! blocks.empty() );
		return blocks[blocks.size() - 1];
	}

	std::shared_ptr<block> blockchain::get_block_by_number(uint64_t num) const {
		if (num >= blocks.size()) {
			return nullptr;
		}
		return std::make_shared<block>(blocks[num]);
	}
	std::shared_ptr<block> blockchain::get_block_by_hash(const std::string& to_find_block_hash) const {
		for (const auto& blk : blocks) {
			const auto& block_hash = blk.block_hash();
			if (block_hash == to_find_block_hash) {
				return std::make_shared<block>(blk);
			}
		}
		return nullptr;
	}
	balance_t blockchain::get_account_asset_balance(const std::string& account_address, asset_id_t asset_id) const {
		auto balances_iter = account_balances.find(account_address);
		if (balances_iter == account_balances.end()) {
			return 0;
		}
		const auto& balances = balances_iter->second;
		auto balance_iter = balances.find(asset_id);
		if (balance_iter == balances.end()) {
			return 0;
		}
		return balance_iter->second;
	}
	void blockchain::update_account_asset_balance(const std::string& account_address, asset_id_t asset_id, int64_t balance_change) {
		auto balances_iter = account_balances.find(account_address);
		std::map<asset_id_t, balance_t> balances;
		if (balances_iter != account_balances.end()) {
			balances = balances_iter->second;
		}
		
		auto balance_iter = balances.find(asset_id);
		if (balance_iter == balances.end()) {
			assert(balance_change >= 0);
			balances[asset_id] = balance_change;
		}
		else {
			assert(balance_change > 0 || (-balance_change <= balance_iter->second));
			balances[asset_id] = balance_t(int64_t(balance_iter->second) + balance_change);
		}
		account_balances[account_address] = balances;
	}
	std::shared_ptr<contract_object> blockchain::get_contract_by_address(const std::string& addr) const {
		for (const auto& it : contracts) {
			if (it.first == addr) {
				return std::make_shared<contract_object>(it.second);
			}
		}
		return nullptr;
	}
	std::shared_ptr<contract_object> blockchain::get_contract_by_name(const std::string& name) const {
		for (const auto& it : contracts) {
			if (it.second.contract_name == name) {
				return std::make_shared<contract_object>(it.second);
			}
		}
		return nullptr;
	}
	void blockchain::store_contract(const std::string& addr, const contract_object& contract_obj) {
		contracts[addr] = contract_obj;
	}

	StorageDataType blockchain::get_storage(const std::string& contract_address, const std::string& key) const {
		auto it1 = contract_storages.find(contract_address);
		if (it1 == contract_storages.end()) {
			std::string null_jsonstr("null");
			return StorageDataType(null_jsonstr);
		}
		auto it2 = it1->second.find(key);
		if (it2 == it1->second.end()) {
			std::string null_jsonstr("null");
			return StorageDataType(null_jsonstr);
		}
		return it2->second;
	}
	void blockchain::set_storage(const std::string& contract_address, const std::string& key, const StorageDataType& value) {
		auto it1 = contract_storages.find(contract_address);
		std::map<std::string, StorageDataType> storages;
		if (it1 != contract_storages.end()) {
			storages = it1->second;
		}
		storages[key] = value;
		contract_storages[contract_address] = storages;
	}

	void blockchain::add_asset(const asset& new_asset) {
		asset item(new_asset);
		item.asset_id = assets.size();
		assets.push_back(item);
	}
	std::shared_ptr<asset> blockchain::get_asset(asset_id_t asset_id) {
		for (const auto& item : assets) {
			if (item.asset_id == asset_id) {
				return std::make_shared<asset>(item);
			}
		}
		return nullptr;
	}
	std::shared_ptr<asset> blockchain::get_asset_by_symbol(const std::string& symbol) {
		for (const auto& item : assets) {
			if (item.symbol == symbol) {
				return std::make_shared<asset>(item);
			}
		}
		return nullptr;
	}

	std::shared_ptr<generic_evaluator> blockchain::get_operation_evaluator(transaction* tx, const operation& op)
	{
		auto type = op.which();
		switch (type) {
		case operation::tag<contract_create_operation>::value: {
			return std::make_shared<contract_create_evaluator>(this, tx);
		} break;
		case operation::tag<contract_invoke_operation>::value: {
			return std::make_shared<contract_invoke_evaluator>(this, tx);
		} break;
		default: {
			auto err = std::string("unknown operation type ") + std::to_string(type);
			throw uvm::core::UvmException(err.c_str());
		}
		}
	}
	void blockchain::generate_block(const std::vector<transaction>& txs) {
		block blk;
		blk.txs = txs;
		blk.block_time = fc::time_point_sec(fc::time_point::now());
		blk.block_number = blocks.size();
		blocks.push_back(blk);
	}
}