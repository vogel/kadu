set (model_SRCS
	actions-proxy-model.cpp
)

set (model_MOC_SRCS
	actions-proxy-model.h
)

qt4_wrap_cpp (model_MOC_FILES ${model_MOC_SRCS})

add_library (model
	STATIC
	${model_SRCS}
	${model_MOC_FILES}
)

set_target_properties (model PROPERTIES COMPILE_DEFINITIONS KADULIB)
