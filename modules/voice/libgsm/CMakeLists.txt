set (GSM_SOURCES 
	src/add.c
	src/code.c
	src/debug.c
	src/decode.c
	src/long_term.c
	src/lpc.c
	src/preprocess.c
	src/rpe.c
	src/gsm_destroy.c
	src/gsm_decode.c
	src/gsm_encode.c
	src/gsm_explode.c
	src/gsm_implode.c
	src/gsm_create.c
	src/gsm_print.c
	src/gsm_option.c
	src/short_term.c
	src/table.c
)

include_directories (inc)

add_library(gsm ${GSM_SOURCES})
set_target_properties(gsm PROPERTIES COMPILE_FLAGS "-DSASR -DUSE_FLOAT_MUL -DFAST -DWAV49" )
