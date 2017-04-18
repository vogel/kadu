set (task_SRCS
	serial-delayed-task-executor.cpp
	task.cpp
	task-module.cpp
)

kadu_subdirectory (task "${task_SRCS}")
