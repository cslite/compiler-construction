all:    testdriver.c lexer/lexer.c utils/hash.c utils/util.c parser/parser.c utils/treeNodePtr_stack.c utils/treeNodePtr_stack_config.c utils/set.c
	gcc testdriver.c lexer/lexer.c utils/hash.c utils/util.c parser/parser.c utils/treeNodePtr_stack.c utils/treeNodePtr_stack_config.c utils/set.c -o cmake/parser.out
