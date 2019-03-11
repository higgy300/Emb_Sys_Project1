#include <utility>

/* On my honor, I have neither given nor received
 unauthorized aid on this assignment */

// for file I/O
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

// for data structures
#include <deque>
#include <vector>
#include <unordered_map>
#include <map>

// for sorting
#include <algorithm>
#include <iterator>

using namespace std;

class MIPS {
public:
    // constructor
    explicit MIPS(string outPut_file) : _step_(0), create_output_file(std::move(outPut_file)), sim_records(""), instruction_index(0) {
        init_register_file();
        init_data_memory();
        init_token_map();
        init_register_map();
        init_data_map();

        // load instructions.txt
        ifstream input_file("instructions.txt");
        string line;
        string expression;
        vector<string> expression_list;
        UndecodedInstruction undecoded_instruction{};
        uint16_t token_limit = 0;

        while (getline(input_file, line)) {
            ++token_limit;
            if (line.empty() || token_limit > 16)
                continue;
            // Filter the expression by removing delimiter
            expression = getExpression(line);
            // Further split expression to obtain single expressions
            expression_list = filter_expression(expression, ',');

            // grab operand type and add it to list
            if (expression_list[0] == "ADD")
                undecoded_instruction.__op__ = OPERAND_ENUM::ADD;
            if (expression_list[0] == "SUB")
                undecoded_instruction.__op__ = OPERAND_ENUM::SUB;
            if (expression_list[0] == "AND")
                undecoded_instruction.__op__ = OPERAND_ENUM::AND;
            if (expression_list[0] == "OR")
                undecoded_instruction.__op__ = OPERAND_ENUM::OR;
            if (expression_list[0] == "LD")
                undecoded_instruction.__op__ = OPERAND_ENUM::LD;

            /* Obtain destination register number from expression and add it to the
               undecoded instruction */
            int destination_register_num = stoi(expression_list[1].substr(1, 1));
            undecoded_instruction.__dest_reg__ = (REGISTER_ENUM)destination_register_num;

            /* Obtain source register number 1 from expression and add it to the
               undecoded instruction */
            int source1_register_num = stoi(expression_list[2].substr(1, 1));
            undecoded_instruction.__src1_reg__ = (REGISTER_ENUM)source1_register_num;

            /* Obtain source register number 2 from expression and add it to the
               undecoded instruction */
            int source2_register_num = stoi(expression_list[3].substr(1, 1));
            undecoded_instruction.__src2_reg__ = (REGISTER_ENUM)source2_register_num;
            undecoded_instruction.line_index = instruction_index++;

            // Now that we have a complete instruction, we push it to the buffer
            INSTRUCTION_MEMORY_BUFFER.push_back(undecoded_instruction);
            expression_list.clear();
        }
        // close instructions.txt
        input_file.close();
        
        token_limit = 0;
        // load registers.txt
        input_file.open("registers.txt");
        vector<string> register_list;

        while (getline(input_file, line)) {
            if (line.empty() || token_limit > 8)
                continue;

            // Filter the expression by removing delimiter
            expression = getExpression(line);
            // Further split expression to obtain single expressions
            expression_list = filter_expression(expression, ',');

            // check if register data is out of bounds (0 - 63)
            int num = stoi(expression_list[1]);
            if (num >= 63) {
                num = 63;
            } else if (num < 0) {
                num = 0;
            }
            /* Extract the register number from the filtered expression and
               push it to the buffer */
            int register_num = stoi(expression_list[0].substr(1, 1));
            REGISTER_FILE_BUFFER[register_num] = num;

            expression_list.clear();
        }
        // close registers.txt
        input_file.close();

        // load datamemory.txt
        input_file.open("datamemory.txt");
        vector<string> data_memory_list;
        token_limit = 0;
        
        while (getline(input_file, line)) {
            if (line.empty() || token_limit > 8)
                continue;

            // Filter the expression by removing delimiter
            expression = getExpression(line);
            // Further split expression to obtain single expressions
            data_memory_list = filter_expression(expression, ',');

            // check if data value is out of bounds (0 - 63)
            int num = stoi(data_memory_list[1]);
            if (num >= 63) {
                num = 63;
            } else if (num < 0) {
                num = 0;
            }

            // Extract data memory value from filtered expression and add it to the buffer
            int data_memory_value = stoi(data_memory_list[0]);
            DATA_MEMORY_BUFFER[data_memory_value] = num;

            data_memory_list.clear();
        }
        // close datamemory.txt
        input_file.close();
    }

    void sim() {
        string curr_step;
        curr_step = fetch_step_info();
        sim_memory_buffer.push_back(curr_step);
        ++_step_;

        while (check_tokens()) {
            // Get ordered indexes and store them
            deque<pair<int, int>> index_list = get_exec_order();

            while (!index_list.empty()) {
                switch (index_list.front().second) {
                    decltype (RESULT_BUFFER)::value_type result_entry;
                    case 0: {
                        // Process instruction memory buffer
                        auto fetch_from_INM = INSTRUCTION_MEMORY_BUFFER.front();

                        // Inspect instruction buffer to retrieve address and
                        decltype (INSTRUCTION_BUFFER)::value_type buffer_entry;

                        // prepare next instruction to be added to buffer
                        buffer_entry.__op__ = fetch_from_INM.__op__;
                        buffer_entry.__dest_reg__ = fetch_from_INM.__dest_reg__;
                        buffer_entry.__reg1_value__ = REGISTER_FILE_BUFFER[fetch_from_INM.__src1_reg__];
                        buffer_entry.__reg2_value__ = REGISTER_FILE_BUFFER[fetch_from_INM.__src2_reg__];
                        buffer_entry.line_index = fetch_from_INM.line_index;

                        // load new instruction to buffer and remove it from memory
                        INSTRUCTION_BUFFER.push_back(buffer_entry);
                        INSTRUCTION_MEMORY_BUFFER.pop_front();
                        index_list.pop_front();
                        break;
                    }
                    case 1: {
                        // Process instruction buffer
                        auto fetch_from_INB = INSTRUCTION_BUFFER.front();

                        // Check if we are loading an instruction from buffer
                        if (fetch_from_INB.__op__ == OPERAND_ENUM::LD) {

                            // inspect LIB to get address
                            decltype (LOAD_INSTRUCTION_BUFFER)::value_type buffer_entry;
                            buffer_entry = fetch_from_INB;

                            // load buffer entry to instruction buffer
                            LOAD_INSTRUCTION_BUFFER.push_back(buffer_entry);
                            // remove it from the instruction buffer
                            INSTRUCTION_BUFFER.pop_front();
                        } else {
                            // inspect AIB to obtain address
                            decltype (ARITHMETIC_INSTRUCTION_BUFFER)::value_type arith_entry;
                            arith_entry = fetch_from_INB;

                            // add instruction to the arithmetic instruction buffer
                            ARITHMETIC_INSTRUCTION_BUFFER.push_back(arith_entry);
                            // remove it from the instruction buffer
                            INSTRUCTION_BUFFER.pop_front();
                        }
                        index_list.pop_front();
                        break;
                    }
                    case 2: {
                        // Process arithmetic instruction buffer
                        auto fetch_from_AIB = ARITHMETIC_INSTRUCTION_BUFFER.front();

                        // Retrieve data from the result buffer and prepare it
                        result_entry.__dest_reg__ = fetch_from_AIB.__dest_reg__;
                        result_entry.__result_value__ = execute_ALU_operation(fetch_from_AIB.__op__,
                                                                              fetch_from_AIB.__reg1_value__,
                                                                              fetch_from_AIB.__reg2_value__);
                        result_entry.line_index = fetch_from_AIB.line_index;

                        // add entry to the result buffer
                        RESULT_BUFFER.push_back(result_entry);
                        // remove it from the arithmetic instruction buffer
                        ARITHMETIC_INSTRUCTION_BUFFER.pop_front();
                        index_list.pop_front();
                        break;
                    }
                    case 3: {
                        // Process load instruction buffer
                        auto fetch_from_LIB = LOAD_INSTRUCTION_BUFFER.front();

                        // inspect address buffer to retrieve address
                        decltype (ADDRESS_BUFFER)::value_type buffer_entry;

                        // prepare entry by fetching from load instruction buffer
                        buffer_entry.__dest_reg__ = fetch_from_LIB.__dest_reg__;
                        buffer_entry.__address__ = (DATA_ENUM)(fetch_from_LIB.__reg1_value__ + fetch_from_LIB.__reg2_value__);
                        buffer_entry.line_index = fetch_from_LIB.line_index;

                        // add new entry to address buffer
                        ADDRESS_BUFFER.push_back(buffer_entry);
                        // remove it from the load instruction buffer
                        LOAD_INSTRUCTION_BUFFER.pop_front();
                        index_list.pop_front();
                        break;
                    }
                    case 4: {
                        // Process adress buffer
                        auto fetch_from_ADB = ADDRESS_BUFFER.front();

                        // prepare new entry fetched from address buffer
                        result_entry.__dest_reg__ = fetch_from_ADB.__dest_reg__;
                        result_entry.__result_value__ = DATA_MEMORY_BUFFER[fetch_from_ADB.__address__];
                        result_entry.line_index = fetch_from_ADB.line_index;

                        // add new entry to result buffer
                        RESULT_BUFFER.push_back(result_entry);
                        // remove it from the address buffer
                        ADDRESS_BUFFER.pop_front();
                        index_list.pop_front();
                        break;
                    }
                    case 5: {
                        // Process results buffer
                        auto fetch_from_REB = RESULT_BUFFER.front();

                        // grab data from register file
                        REGISTER_FILE_BUFFER[fetch_from_REB.__dest_reg__] = fetch_from_REB.__result_value__;

                        // remove it from the results buffer
                        RESULT_BUFFER.pop_front();
                        index_list.pop_front();
                        break;
                    }
                }

            }
            curr_step = fetch_step_info();
            sim_memory_buffer.push_back(curr_step);
            ++_step_;
        }
        dump_sim_result_to_buffer();
    }

    void write_to_output_file() {
        ofstream os(create_output_file);
        os << sim_records;
        os.close();
    }

    string print_results() {
        return sim_records;
    }

private:
    int _step_;
    int instruction_index;
    string sim_records; // for debugging
    string create_output_file;

    deque<string> sim_memory_buffer;
    map<int, int> REGISTER_FILE_BUFFER;
    map<int, int> DATA_MEMORY_BUFFER;
    unordered_map<int, string> _register_map_;
    unordered_map<int, string> _operand_map_;
    unordered_map<int, string> _data_map_;

    enum OPERAND_ENUM { ADD, SUB, AND, OR, LD };
    enum REGISTER_ENUM { R0, R1, R2, R3, R4, R5, R6, R7 };
    enum DATA_ENUM { D0, D1, D2, D3, D4, D5, D6, D7 };

    class Address {
    public:
        REGISTER_ENUM __dest_reg__;
        DATA_ENUM __address__;
        int line_index;
    };

    deque<Address> ADDRESS_BUFFER;

    class DecodedInstruction {
    public:
        OPERAND_ENUM __op__;
        REGISTER_ENUM __dest_reg__;
        int __reg1_value__;
        int __reg2_value__;
        int line_index;
    };

    deque<DecodedInstruction> INSTRUCTION_BUFFER;
    deque<DecodedInstruction> ARITHMETIC_INSTRUCTION_BUFFER;
    deque<DecodedInstruction> LOAD_INSTRUCTION_BUFFER;

    class Results {
    public:
        REGISTER_ENUM __dest_reg__;
        int __result_value__;
        int line_index;
    };

    deque<Results> RESULT_BUFFER;

    class UndecodedInstruction {
    public:
        OPERAND_ENUM __op__;
        REGISTER_ENUM __dest_reg__;
        REGISTER_ENUM __src1_reg__;
        REGISTER_ENUM __src2_reg__;
        int line_index;
    };

    deque<UndecodedInstruction> INSTRUCTION_MEMORY_BUFFER;

    int execute_ALU_operation(OPERAND_ENUM operand, int data_a, int data_b) {
        int result = 0;
        switch (operand) {
            case 0:
                result = data_a + data_b;
                if (data_a + data_b >= 63) {
                    while (result > 63) {
                        result -= 63;
                    }
                }
                break;
            case 1:
                result = data_a - data_b;
                if (data_a - data_b < 0) {
                    result = 0;
                }
                break;
            case 2:
                return (data_a & data_b);
            case 3:
                return (data_a | data_b);
            case 4:
                result = data_a + data_b;
                if (data_a + data_b >= 63) {
                    while (result > 63) {
                        result -= 63;
                    }
                }
                if (data_a + data_b < 0) {
                    result = 0;
                }
                break;
        }
    }

    void init_data_memory() {
        int i = 0;
        while (i <= 7) {
            DATA_MEMORY_BUFFER.insert(make_pair(i++, 0));
        }
    }

    void init_register_file() {
        int i = 0;

        while (i <= 7) {
            REGISTER_FILE_BUFFER.insert(make_pair(i++, 0));
        }
    }

    void init_register_map() {
        int i = 0;

        while (i <= 7) {
            _register_map_.insert(make_pair(i, "R" + to_string(i)));
            i++;
        }
    }

    void init_data_map() {
        int i = 0;

        while (i <= 7) {
            _data_map_.insert(make_pair(i, "D" + to_string(i)));
            i++;
        }
    }

    void init_token_map() {
        _operand_map_.insert(make_pair(0, "ADD"));
        _operand_map_.insert(make_pair(1, "SUB"));
        _operand_map_.insert(make_pair(2, "AND"));
        _operand_map_.insert(make_pair(3, "OR"));
        _operand_map_.insert(make_pair(4, "LD"));
    }

    bool check_tokens() {
        return !INSTRUCTION_MEMORY_BUFFER.empty() || !INSTRUCTION_BUFFER.empty() ||
               !ARITHMETIC_INSTRUCTION_BUFFER.empty() || !LOAD_INSTRUCTION_BUFFER.empty() ||
               !ADDRESS_BUFFER.empty() || !RESULT_BUFFER.empty();
    }

    string getExpression(const string &str, const string &left = "<", const string &right = ">") {
        return str.substr(1, str.length() - 2);
    }

    deque<pair<int, int>> get_exec_order() {
        deque<pair<int, int>> _list;

        if (!INSTRUCTION_MEMORY_BUFFER.empty())
            _list.emplace_back(INSTRUCTION_MEMORY_BUFFER.front().line_index, 0);

        if (!INSTRUCTION_BUFFER.empty())
            _list.emplace_back(INSTRUCTION_BUFFER.front().line_index, 1);

        if (!ARITHMETIC_INSTRUCTION_BUFFER.empty())
            _list.emplace_back(ARITHMETIC_INSTRUCTION_BUFFER.front().line_index, 2);

        if (!LOAD_INSTRUCTION_BUFFER.empty())
            _list.emplace_back(LOAD_INSTRUCTION_BUFFER.front().line_index, 3);

        if (!ADDRESS_BUFFER.empty())
            _list.emplace_back(ADDRESS_BUFFER.front().line_index, 4);

        if (!RESULT_BUFFER.empty())
            _list.emplace_back(RESULT_BUFFER.front().line_index, 5);

        sort(_list.begin(), _list.end(), [](pair<int, int> const& param1, pair<int, int>  const param2) {
            return (param1.first < param2.first);
        });
        return _list;
    }

    string fetch_step_info() {
        string step_str = "STEP " + to_string(_step_) + ':';
        string arith_ins_buffer_str = "AIB:";
        string ins_buffer_str = "INB:";
        string load_ins_buffer_str = "LIB:";
        string addr_buffer_str = "ADB:";
        string data_mem_str = "DAM:";
        string ins_mem_str = "INM:";
        string result_buffer_str = "REB:";
        string reg_file_str = "RGF:";

        // instruction memory initialization
        string ins_mem_entry = "";
        vector<string> ins_mem;

        for (const auto& entry : INSTRUCTION_MEMORY_BUFFER) {
            ins_mem_entry = '<' + _operand_map_[entry.__op__] + ',';
            ins_mem_entry += _register_map_[entry.__dest_reg__] + ',';
            ins_mem_entry += _register_map_[entry.__src1_reg__] + ',';
            ins_mem_entry += _register_map_[entry.__src2_reg__] + '>';
            ins_mem.push_back(ins_mem_entry);
        }

        string ins_mem_ = "";

        for (const auto& entry : ins_mem)
            ins_mem_ += entry + ',';

        ins_mem_ = (ins_mem_ == "") ? ins_mem_ : ins_mem_.substr(0, ins_mem_.length() - 1);
        ins_mem_ = ins_mem_str + ins_mem_;

        // instruction buffer initialization
        string ins_buffer_entry = "";
        vector<string> ins_buff;

        for (const auto& entry : INSTRUCTION_BUFFER) {
            ins_buffer_entry = '<' + _operand_map_[entry.__op__] + ',';
            ins_buffer_entry += _register_map_[entry.__dest_reg__] + ',';
            ins_buffer_entry += to_string(entry.__reg1_value__) + ',';
            ins_buffer_entry += to_string(entry.__reg2_value__) + '>';
            ins_buff.push_back(ins_buffer_entry);
        }

        string ins_buff_ = "";

        for (const auto& entry : ins_buff)
            ins_buff_ += entry + ',';

        ins_buff_ = (ins_buff_.empty()) ? ins_buff_ : ins_buff_.substr(0, ins_buff_.length() - 1);
        ins_buff_ = ins_buffer_str + ins_buff_;

        // arithmetic instruction initialization
        string arith_ins_buffer_entry = "";
        vector<string> arith_ins_buff;

        for (const auto& entry : ARITHMETIC_INSTRUCTION_BUFFER) {
            arith_ins_buffer_entry = '<' + _operand_map_[entry.__op__] + ',';
            arith_ins_buffer_entry += _register_map_[entry.__dest_reg__] + ',';
            arith_ins_buffer_entry += to_string(entry.__reg1_value__) + ',';
            arith_ins_buffer_entry += to_string(entry.__reg2_value__) + '>';
            arith_ins_buff.push_back(arith_ins_buffer_entry);
        }

        string arith_ins = "";

        for (const auto& entry : arith_ins_buff)
            arith_ins += entry + ',';

        arith_ins = (arith_ins.empty()) ? arith_ins : arith_ins.substr(0, arith_ins.length() - 1);
        arith_ins = arith_ins_buffer_str + arith_ins;

        // load instruction buffer initialization
        string load_ins_buffer_entry = "";
        vector<string> load_ins_buffer;

        for (const auto& entry : LOAD_INSTRUCTION_BUFFER) {
            load_ins_buffer_entry = '<' + _operand_map_[entry.__op__] + ',';
            load_ins_buffer_entry += _register_map_[entry.__dest_reg__] + ',';
            load_ins_buffer_entry += to_string(entry.__reg1_value__) + ',';
            load_ins_buffer_entry += to_string(entry.__reg2_value__) + '>';
            load_ins_buffer.push_back(load_ins_buffer_entry);
        }

        string load_ins = "";

        for (const auto& entry : load_ins_buffer)
            load_ins += entry + ',';

        load_ins = (load_ins.empty()) ? load_ins : load_ins.substr(0, load_ins.length() - 1);
        load_ins = load_ins_buffer_str + load_ins;

        // address buffer initialization
        string addr_buffer_entry;
        vector<string> addr_buffer;
        for (const auto& entry : ADDRESS_BUFFER) {
            addr_buffer_entry = '<' + _register_map_[entry.__dest_reg__] + ',';
            addr_buffer_entry += to_string(entry.__address__) + '>';
            addr_buffer.push_back(addr_buffer_entry);
        }

        string addr_buff = "";

        for (const auto& entry : addr_buffer)
            addr_buff += entry + ',';

        addr_buff = (addr_buff.empty()) ? addr_buff : addr_buff.substr(0, addr_buff.length() - 1);
        addr_buff = addr_buffer_str + addr_buff;

        // result buffer initialization
        string res_buffer_entry = "";
        vector<string> res_buffer;

        for (const auto& entry : RESULT_BUFFER) {
            res_buffer_entry = '<' + _register_map_[entry.__dest_reg__] + ',';
            res_buffer_entry += to_string(entry.__result_value__) + '>';
            res_buffer.push_back(res_buffer_entry);
        }

        string res_buff = "";

        for (const auto& entry : res_buffer)
            res_buff += entry + ',';

        res_buff = (res_buff.empty()) ? res_buff : res_buff.substr(0, res_buff.length() - 1);
        res_buff = result_buffer_str + res_buff;

        // register file initialization
        string reg_file_entry;
        vector<string> reg_file;

        for (const auto& entry : REGISTER_FILE_BUFFER) {
            reg_file_entry = '<' + _register_map_[entry.first] + ',';
            reg_file_entry += to_string(entry.second) + '>';
            reg_file.push_back(reg_file_entry);
        }

        string reg_file_ = "";

        for (const auto& entry : reg_file)
            reg_file_ += entry + ',';

        reg_file_ = reg_file_.substr(0, reg_file_.length() - 1);
        reg_file_ = reg_file_str + reg_file_;

        // data memory initialization
        string data_mem_entry;
        vector<string> data_mem;

        for (const auto& entry : DATA_MEMORY_BUFFER) {
            data_mem_entry = '<' + to_string(entry.first) + ',';
            data_mem_entry += to_string(entry.second) + '>';
            data_mem.push_back(data_mem_entry);
        }

        string data_mem_ = "";

        for (const auto& entry : data_mem)
            data_mem_ += entry + ',';

        data_mem_ = data_mem_.substr(0, data_mem_.length() - 1);
        data_mem_ = data_mem_str + data_mem_;

        string result = "";
        result = step_str + '\n' + ins_mem_ + '\n' + ins_buff_ + '\n' +
                 arith_ins + '\n' + load_ins + '\n' + addr_buff + '\n' +
                 res_buff + '\n' + reg_file_ + '\n' + data_mem_;
        return result;
    }

    void dump_sim_result_to_buffer() {

        for (auto const& submission : sim_memory_buffer)
            sim_records += submission + '\n' + '\n';

        sim_records = (sim_records.empty()) ? sim_records : sim_records.substr(0, sim_records.length() - 2);
    }

    template<typename T>
    void filter_expression(const string &str, char charToRemove, T ptr) {
        stringstream line;
        line.str(str);
        string temp;
        while (getline(line, temp, charToRemove)) {
            *(ptr++) = temp;
        }
    }

    vector<string> filter_expression(const string &str, char charToRemove) {
        vector<string> temp;
        filter_expression(str, charToRemove, back_inserter(temp));
        return temp;
    }
};

int main() {
    MIPS init("simulation.txt");
    init.sim();
    init.write_to_output_file();
    return 0;
}