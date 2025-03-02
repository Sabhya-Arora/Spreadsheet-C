TARGET_EXEC := target/release/spreadsheet



BUILD_DIR := ./build

TEST_BUILD_DIR := $(BUILD_DIR)/tests

SRC_DIRS := ./src

TEST_DIR := ./tests



# Find all the C and C++ files we want to compile

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

DEPS := $(OBJS:.o=.d)



INC_DIRS := $(shell find $(SRC_DIRS) -type d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -MMD -MP



$(TARGET_EXEC): $(OBJS)

	mkdir -p $(dir $@)

	$(CXX) $(OBJS) -o $@ $(LDFLAGS)



$(BUILD_DIR)/%.c.o: %.c

	mkdir -p $(dir $@)

	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@



$(BUILD_DIR)/%.cpp.o: %.cpp

	mkdir -p $(dir $@)

	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@



.PHONY: clean test



clean:

	rm -rf $(BUILD_DIR) target
	rm -f *.aux *.log *.out *.toc *.lof *.lot *.bbl *.blg *.dvi *.fls *.fdb_latexmk $(REPORT_PDF)
	rm -rf $(REPORT_DIR)


# Unit Testing

CHECK_LIBS = -lcheck -lm -lpthread -lrt -lsubunit

TEST_SRC = tests/test_spreadsheet.c

TEST_EXEC = $(TEST_BUILD_DIR)/test_spreadsheet



# Exclude C_lab.c from test compilation

MAIN_SRCS := $(filter-out ./src/C_lab.c, $(SRCS))



$(TEST_EXEC): $(TEST_SRC) $(MAIN_SRCS)

	mkdir -p $(TEST_BUILD_DIR)  # Ensure build/tests directory exists

	$(CC) $(CPPFLAGS) $(CFLAGS) $(TEST_SRC) $(MAIN_SRCS) -o $@ $(CHECK_LIBS)  # Correct output path



test: $(TEST_EXEC)

	./$(TEST_EXEC)


REPORT_DIR := build/report
SRC_REPORT_DIR := src/report
REPORT_TEX := $(SRC_REPORT_DIR)/report.tex
REPORT_PDF := report.pdf
LATEX := pdflatex
LATEX_OPTS := -interaction=nonstopmode -halt-on-error
FIGURES := $(SRC_REPORT_DIR)/figure.jpeg

.PHONY: report cleanreport

$(REPORT_DIR):
	mkdir -p $(REPORT_DIR)

report: $(REPORT_PDF)

$(REPORT_PDF): $(REPORT_TEX) | $(REPORT_DIR)
	$(LATEX) $(LATEX_OPTS) -output-directory=$(REPORT_DIR) $(REPORT_TEX)
	$(LATEX) $(LATEX_OPTS) -output-directory=$(REPORT_DIR) $(REPORT_TEX)
	mv $(REPORT_DIR)/report.pdf .