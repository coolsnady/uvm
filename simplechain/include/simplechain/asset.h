#pragma once
#include <simplechain/config.h>

namespace simplechain {
	typedef uint32_t asset_id_t;
	struct asset {
		asset_id_t asset_id;
		std::string symbol;
		uint32_t precision;
	};
}

FC_REFLECT(simplechain::asset, (asset_id)(symbol)(precision))
