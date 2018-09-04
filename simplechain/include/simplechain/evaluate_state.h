#pragma once
#include <simplechain/config.h>
#include <simplechain/contract_entry.h>
#include <simplechain/storage.h>
#include <simplechain/contract.h>
#include <simplechain/contract_object.h>

namespace simplechain {
	class blockchain;
	struct transaction;

	struct evaluate_state {
		gas_count_type gas_limit = 0;
		gas_count_type gas_used = 0;
		std::string caller_address;
		contract_invoke_result invoke_contract_result;
		blockchain* chain = nullptr;
		transaction* current_tx = nullptr;

		evaluate_state(blockchain* chain_, transaction* tx_);
		virtual ~evaluate_state() {}

		blockchain* get_chain() const;
		virtual void set_current_tx(transaction* tx);
		virtual transaction* get_current_tx() const;

		// TODO: get balance/state/storage from evaluate_state cache + blockchain storage

		StorageDataType get_storage(const std::string& contract_address, const std::string& key) const;
		void emit_event(const std::string& contract_address, const std::string& event_name, const std::string& event_arg);
		void store_contract(const std::string& contract_address,
			const contract_object& contract_obj);
		void set_contract_storage_changes(const std::string& contract_address, const contract_storage_changes_type& changes);
	};
}