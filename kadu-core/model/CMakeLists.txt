set (model_SRCS
	action-filter-proxy-model.cpp
	action-list-model.cpp
	categorized-sort-filter-proxy-model.cpp
	kadu-merged-proxy-model.cpp
	merged-proxy-model.cpp
	merged-proxy-model-factory.cpp
	model-chain.cpp
	model-index-list-converter.cpp
	model-watcher.cpp
)

kadu_subdirectory (model "${model_SRCS}" "" "")
