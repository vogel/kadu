set (model_SRCS
	actions-proxy-model.cpp
	model-chain.cpp
	model-index-list-converter.cpp
)

set (model_MOC_SRCS
	actions-proxy-model.h
	model-chain.h
)

kadu_subdirectory (model "${model_SRCS}" "${model_MOC_SRCS}" "")
