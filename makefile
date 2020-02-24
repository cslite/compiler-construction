all:    lexer.c lexer.h lexerDef.h parser.c parser.h parserDef.h set.c set.h hash.h hash.c driver.c util.c util.h config.h treeNodePtr_stack.c treeNodePtr_stack.h treeNodePtr_stack_config.h treeNodePtr_stack_config.c error.c error.h errorPtr_stack.c errorPtr_stack.h errorPtr_stack_config.c errorPtr_stack_config.h
	gcc lexer.c parser.c set.c hash.c driver.c util.c treeNodePtr_stack.c treeNodePtr_stack_config.c error.c errorPtr_stack.c errorPtr_stack_config.c -o stage1exe
