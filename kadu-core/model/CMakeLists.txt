set (model_SRCS
	action-filter-proxy-model.cpp
	action-list-model.cpp
	kadu-merged-proxy-model.cpp
	merged-proxy-model.cpp
	merged-proxy-model-factory.cpp
	model-chain.cpp
	model-index-list-converter.cpp
	model-watcher.cpp
)

set (model_MOC_SRCS
	action-filter-proxy-model.h
	action-list-model.h
	kadu-merged-proxy-model.h
	merged-proxy-model.h
	model-chain.h
	model-watcher.h
)

kadu_subdirectory (model "${model_SRCS}" "${model_MOC_SRCS}" "")
