set (model_SRCS
	actions-proxy-model.cpp
)

set (model_MOC_SRCS
	actions-proxy-model.h
)

kadu_subdirectory (model "${model_SRCS}" "${model_MOC_SRCS}" "")
