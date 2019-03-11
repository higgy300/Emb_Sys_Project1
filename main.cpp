/* On my honor, I have neither given nor received
 unauthorized aid on this assignment */

/*The only code that was taken from online resources are the following accessory functions:
std::string get_BetweenDelimter()
std::vector <std::string> split()

These functions help in parsing the input data ONLY.
*/

#include<iostream>
#include <vector>
#include <deque>
#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <algorithm>

class M_Sim
{
private:

	enum operand { ADD, SUB, AND, OR, LD };
	enum regis { R0, R1, R2, R3, R4, R5, R6, R7 };
	enum datad { D0, D1, D2, D3, D4, D5, D6, D7 };

	std::unordered_map<int, std::string> oMap;
	std::unordered_map<int, std::string> rMap;
	std::unordered_map<int, std::string> dMap;

	class rawINST
	{
	public:
		operand op;
		regis destReg;
		regis sourceReg_1;
		regis sourceReg_2;
		int lIndex;
	};

	class decodedINST
	{
	public:
		operand op;
		regis destReg;
		int val_1;
		int val_2;
		int lIndex;
	};

	class resultENTRY
	{
	public:
		regis destReg;
		int val_1;
		int lIndex;
	};

	class addrENTRY
	{
	public:
		regis destReg;
		datad addr;
		int lIndex;
	};


	void ready_RGF()
	{
		int itr = 0;
		while (itr <= 7)
		{
			RGF.insert(std::make_pair(itr++, 0));
		}
	}

	void ready_DAM()
	{
		int itr = 0;
		while (itr <= 7)
		{
			DAM.insert(std::make_pair(itr++, 0));
		}
	}

	void ready_oMap()
	{
		oMap.insert(std::make_pair(0, "ADD"));
		oMap.insert(std::make_pair(1, "SUB"));
		oMap.insert(std::make_pair(2, "AND"));
		oMap.insert(std::make_pair(3, "OR"));
		oMap.insert(std::make_pair(4, "LD"));
	}

	void ready_rMap()
	{
		int itr = 0;
		while (itr <= 7)
		{
			rMap.insert(std::make_pair(itr, "R" + std::to_string(itr)));
			itr++;
		}
	}

	void ready_dMap()
	{
		int itr = 0;
		while (itr <= 7)
		{
			dMap.insert(std::make_pair(itr, "D" + std::to_string(itr)));
			itr++;
		}
	}

	std::string get_BetweenDelimter(std::string line, std::string del_1 = "<", std::string del_2 = ">")
	{
		/*	unsigned first = line.find(del_1);
		unsigned last = line.find(del_2);
		std::string retS = line.substr(first, last - first);
		return retS;*/
		std::string retString = line.substr(1, line.length() - 2);
		return retString;
	}

	template<typename Out>
	void split(const std::string &s, char delim, Out result) {
		std::stringstream ss;
		ss.str(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			*(result++) = item;
		}
	}


	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
	}

	bool isTokensRemaining()
	{
		if (INM.size() > 0 || INB.size() > 0 || AIB.size() > 0 || LIB.size() > 0 || ADB.size() > 0 || REB.size() > 0)
			return true;
		else
			return false;
	}

	std::string generateCurrentStep()
	{
		std::string stepHEADER = "STEP " + std::to_string(timeSTEP) + ':';
		std::string inmHEADER = "INM:";
		std::string inbHEADER = "INB:";
		std::string aibHEADER = "AIB:";
		std::string libHEADER = "LIB:";
		std::string adbHEADER = "ADB:";
		std::string rebHEADER = "REB:";
		std::string rgfHEADER = "RGF:";
		std::string damHEADER = "DAM:";

		//INM construction##################################
		std::string inmEntry = "";
		std::vector<std::string> inmEntryVector;
		for (const auto& entry : INM)
		{
			inmEntry = '<' + oMap[entry.op] + ',';
			inmEntry += rMap[entry.destReg] + ',';
			inmEntry += rMap[entry.sourceReg_1] + ',';
			inmEntry += rMap[entry.sourceReg_2] + '>';

			inmEntryVector.push_back(inmEntry);
		}

		std::string inmActual = "";
		for (const auto& entry : inmEntryVector)
			inmActual += entry + ',';
		inmActual = (inmActual == "") ? inmActual : inmActual.substr(0, inmActual.length() - 1);

		inmActual = inmHEADER + inmActual;
		//####################################################

		//INB construction##################################
		std::string inbEntry = "";
		std::vector<std::string> inbEntryVector;
		for (const auto& entry : INB)
		{
			inbEntry = '<' + oMap[entry.op] + ',';
			inbEntry += rMap[entry.destReg] + ',';
			inbEntry += std::to_string(entry.val_1) + ',';
			inbEntry += std::to_string(entry.val_2) + '>';

			inbEntryVector.push_back(inbEntry);
		}

		std::string inbActual = "";
		for (const auto& entry : inbEntryVector)
			inbActual += entry + ',';
		inbActual = (inbActual == "") ? inbActual : inbActual.substr(0, inbActual.length() - 1);

		inbActual = inbHEADER + inbActual;
		//####################################################

		//AIB construction##################################
		std::string aibEntry = "";
		std::vector<std::string> aibEntryVector;
		for (const auto& entry : AIB)
		{
			aibEntry = '<' + oMap[entry.op] + ',';
			aibEntry += rMap[entry.destReg] + ',';
			aibEntry += std::to_string(entry.val_1) + ',';
			aibEntry += std::to_string(entry.val_2) + '>';

			aibEntryVector.push_back(aibEntry);
		}

		std::string aibActual = "";
		for (const auto& entry : aibEntryVector)
			aibActual += entry + ',';
		aibActual = (aibActual == "") ? aibActual : aibActual.substr(0, aibActual.length() - 1);

		aibActual = aibHEADER + aibActual;
		//####################################################

		//LIB construction##################################
		std::string libEntry = "";
		std::vector<std::string> libEntryVector;
		for (const auto& entry : LIB)
		{
			libEntry = '<' + oMap[entry.op] + ',';
			libEntry += rMap[entry.destReg] + ',';
			libEntry += std::to_string(entry.val_1) + ',';
			libEntry += std::to_string(entry.val_2) + '>';

			libEntryVector.push_back(libEntry);
		}

		std::string libActual = "";
		for (const auto& entry : libEntryVector)
			libActual += entry + ',';
		libActual = (libActual == "") ? libActual : libActual.substr(0, libActual.length() - 1);

		libActual = libHEADER + libActual;
		//####################################################

		//ADB construction##################################
		std::string adbEntry;
		std::vector<std::string> adbEntryVector;
		for (const auto& entry : ADB)
		{
			adbEntry = '<' + rMap[entry.destReg] + ',';
			adbEntry += std::to_string(entry.addr) + '>';

			adbEntryVector.push_back(adbEntry);
		}

		std::string adbActual = "";
		for (const auto& entry : adbEntryVector)
			adbActual += entry + ',';
		adbActual = (adbActual == "") ? adbActual : adbActual.substr(0, adbActual.length() - 1);

		adbActual = adbHEADER + adbActual;
		//####################################################

		//REB construction##################################
		std::string rebEntry = "";
		std::vector<std::string> rebEntryVector;
		for (const auto& entry : REB)
		{
			rebEntry = '<' + rMap[entry.destReg] + ',';
			rebEntry += std::to_string(entry.val_1) + '>';

			rebEntryVector.push_back(rebEntry);
		}

		std::string rebActual = "";
		for (const auto& entry : rebEntryVector)
			rebActual += entry + ',';
		rebActual = (rebActual == "") ? rebActual : rebActual.substr(0, rebActual.length() - 1);

		rebActual = rebHEADER + rebActual;
		//####################################################

		//RGF construction##################################
		std::string rgfEntry;
		std::vector<std::string> rgfEntryVector;
		for (const auto& entry : RGF)
		{
			rgfEntry = '<' + rMap[entry.first] + ',';
			rgfEntry += std::to_string(entry.second) + '>';

			rgfEntryVector.push_back(rgfEntry);
		}

		std::string rgfActual = "";
		for (const auto& entry : rgfEntryVector)
			rgfActual += entry + ',';
		rgfActual = rgfActual.substr(0, rgfActual.length() - 1);

		rgfActual = rgfHEADER + rgfActual;
		//####################################################

		//DAM construction##################################
		std::string damEntry;
		std::vector<std::string> damEntryVector;
		for (const auto& entry : DAM)
		{
			damEntry = '<' + std::to_string(entry.first) + ',';
			damEntry += std::to_string(entry.second) + '>';

			damEntryVector.push_back(damEntry);
		}

		std::string damActual = "";
		for (const auto& entry : damEntryVector)
			damActual += entry + ',';
		damActual = damActual.substr(0, damActual.length() - 1);

		damActual = damHEADER + damActual;
		//####################################################

		std::string retString = "";

		retString =
			stepHEADER + '\n' +
			inmActual + '\n' +
			inbActual + '\n' +
			aibActual + '\n' +
			libActual + '\n' +
			adbActual + '\n' +
			rebActual + '\n' +
			rgfActual + '\n' +
			damActual;

		return retString;
	}

	int processOperation(operand op, int val_1, int val_2)
	{
		switch (op)
		{
		case 0:
			return (val_1 + val_2);
			break;
		case 1:
			return (val_1 - val_2);
			break;
		case 2:
			return (val_1 & val_2);
			break;
		case 3:
			return (val_1 | val_2);
			break;
		case 4:
			return (val_1 + val_2);
			break;
		}

	}

	std::deque<std::pair<int, int>> getOrderedIndex()
	{
		std::deque<std::pair<int, int>> orderedIndexes;

		if (INM.size() > 0)
			orderedIndexes.push_back(std::make_pair(INM.front().lIndex, 0));

		if (INB.size() > 0)
			orderedIndexes.push_back(std::make_pair(INB.front().lIndex, 1));

		if (AIB.size() > 0)
			orderedIndexes.push_back(std::make_pair(AIB.front().lIndex, 2));

		if (LIB.size() > 0)
			orderedIndexes.push_back(std::make_pair(LIB.front().lIndex, 3));

		if (ADB.size() > 0)
			orderedIndexes.push_back(std::make_pair(ADB.front().lIndex, 4));

		if (REB.size() > 0)
			orderedIndexes.push_back(std::make_pair(REB.front().lIndex, 5));

		std::sort(orderedIndexes.begin(), orderedIndexes.end(), [](std::pair<int, int> const& el1, std::pair<int, int>  const el2)
		{ return (el1.first < el2.first); });

		return orderedIndexes;

	}

	void prepareReport()
	{
		for (auto const& entry : partSimulationStorage)
			simulationStorage += entry + '\n' + '\n';
		simulationStorage = (simulationStorage == "") ? simulationStorage : simulationStorage.substr(0, simulationStorage.length() - 2);
	}

	int timeSTEP;
	int iIndex;

	std::deque<rawINST> INM;
	std::deque<decodedINST> INB;
	std::deque<decodedINST> AIB;
	std::deque<decodedINST> LIB;
	std::deque<addrENTRY> ADB;
	std::deque<resultENTRY> REB;

	std::map<int, int> RGF;
	std::map<int, int> DAM;

	std::deque<std::string> partSimulationStorage;
	std::string simulationStorage;
	std::string WriteTo_File;

public:

	M_Sim(std::string instruction_file, std::string register_file, std::string data_file, std::string outPut_file) : timeSTEP(0), WriteTo_File(outPut_file), simulationStorage(""), iIndex(0)
	{
		ready_RGF();
		ready_DAM();
		ready_oMap();
		ready_rMap();
		ready_dMap();

		//Process Instructions
		std::ifstream ff(instruction_file);
		std::string line;
		std::vector<std::string> tokenizedINST;
		std::string instMSQ;
		rawINST rawInstruction;

		while (std::getline(ff, line))
		{
			if (line == "") continue;

			instMSQ = get_BetweenDelimter(line);
			tokenizedINST = split(instMSQ, ',');

			if (tokenizedINST[0] == "ADD")  rawInstruction.op = operand::ADD;
			if (tokenizedINST[0] == "SUB")  rawInstruction.op = operand::SUB;
			if (tokenizedINST[0] == "AND")  rawInstruction.op = operand::AND;
			if (tokenizedINST[0] == "OR")  rawInstruction.op = operand::OR;
			if (tokenizedINST[0] == "LD")  rawInstruction.op = operand::LD;

			int destRegNum = std::stoi(tokenizedINST[1].substr(1, 1));
			rawInstruction.destReg = (regis)destRegNum;

			int srcRegNum_1 = std::stoi(tokenizedINST[2].substr(1, 1));
			rawInstruction.sourceReg_1 = (regis)srcRegNum_1;

			int srcRegNum_2 = std::stoi(tokenizedINST[3].substr(1, 1));
			rawInstruction.sourceReg_2 = (regis)srcRegNum_2;

			rawInstruction.lIndex = iIndex++;

			INM.push_back(rawInstruction);
			tokenizedINST.clear();
		}
		ff.close();

		//Process Register File
		ff.open(register_file);
		std::vector<std::string> tokenizedREG;

		while (std::getline(ff, line))
		{
			if (line == "") continue;

			instMSQ = get_BetweenDelimter(line);
			tokenizedINST = split(instMSQ, ',');

			int destRegNum = std::stoi(tokenizedINST[0].substr(1, 1));
			RGF[destRegNum] = std::stoi(tokenizedINST[1]);

			tokenizedINST.clear();
		}
		ff.close();

		//Process Data Address File
		ff.open(data_file);
		std::vector<std::string> tokenizedDAM;

		while (std::getline(ff, line))
		{
			if (line == "") continue;

			instMSQ = get_BetweenDelimter(line);
			tokenizedDAM = split(instMSQ, ',');

			int destDamNum = std::stoi(tokenizedDAM[0]);
			DAM[destDamNum] = std::stoi(tokenizedDAM[1]);

			tokenizedDAM.clear();
		}
		ff.close();
	}


	void runSimulation()
	{
		std::string currentSnapShot;

		currentSnapShot = generateCurrentStep();
		partSimulationStorage.push_back(currentSnapShot);
		timeSTEP++;

		while (isTokensRemaining())
		{
			//Caculate order
			std::deque<std::pair<int, int>> orderedIndexes = getOrderedIndex();

			while (orderedIndexes.size() > 0)
			{
				switch (orderedIndexes.front().second)
				{
					decltype (REB)::value_type rebEntry;

				case 0:
				{
					//Process INM
					auto inmToProcess = INM.front();
					decltype (INB)::value_type inbEntry;

					inbEntry.op = inmToProcess.op;
					inbEntry.destReg = inmToProcess.destReg;
					inbEntry.val_1 = RGF[inmToProcess.sourceReg_1];
					inbEntry.val_2 = RGF[inmToProcess.sourceReg_2];
					inbEntry.lIndex = inmToProcess.lIndex;

					INB.push_back(inbEntry);
					INM.pop_front();

					orderedIndexes.pop_front();
					break;
				}
				case 1:
				{
					//Process INB
					auto inbToProcess = INB.front();

					if (inbToProcess.op == operand::LD)
					{
						decltype (LIB)::value_type libEntry;
						libEntry = inbToProcess;

						LIB.push_back(libEntry);
						INB.pop_front();
					}
					else
					{
						decltype (AIB)::value_type aibEntry;
						aibEntry = inbToProcess;

						AIB.push_back(aibEntry);
						INB.pop_front();
					}
					orderedIndexes.pop_front();
					break;
				}
				case 2:
				{
					//Process AIB
					auto aibToProcess = AIB.front();

					rebEntry.destReg = aibToProcess.destReg;
					rebEntry.val_1 = processOperation(aibToProcess.op, aibToProcess.val_1, aibToProcess.val_2);
					rebEntry.lIndex = aibToProcess.lIndex;

					REB.push_back(rebEntry);
					AIB.pop_front();

					orderedIndexes.pop_front();
					break;
				}
				case 3:
				{
					//Process LIB
					auto libToProcess = LIB.front();
					decltype (ADB)::value_type adbEntry;

					adbEntry.destReg = libToProcess.destReg;
					adbEntry.addr = (datad)(libToProcess.val_1 + libToProcess.val_2);
					adbEntry.lIndex = libToProcess.lIndex;

					ADB.push_back(adbEntry);
					LIB.pop_front();

					orderedIndexes.pop_front();
					break;
				}
				case 4:
				{
					//Process ADB
					auto adbToProcess = ADB.front();

					rebEntry.destReg = adbToProcess.destReg;
					rebEntry.val_1 = DAM[adbToProcess.addr];
					rebEntry.lIndex = adbToProcess.lIndex;

					REB.push_back(rebEntry);
					ADB.pop_front();

					orderedIndexes.pop_front();
					break;
				}
				case 5:
				{
					//Process REB
					auto rebToProcess = REB.front();

					RGF[rebToProcess.destReg] = rebToProcess.val_1;

					REB.pop_front();

					orderedIndexes.pop_front();
					break;
				}
				}

			}
			currentSnapShot = generateCurrentStep();
			partSimulationStorage.push_back(currentSnapShot);

			timeSTEP++;
		}
		prepareReport();
	}

	void writeReportToFile()
	{
		std::ofstream ofsWriter(WriteTo_File);
		ofsWriter << simulationStorage;
		ofsWriter.close();
	}

	std::string showReport()
	{
		return simulationStorage;
	}


};

int main()
{
	const std::string regFile = "registers.txt";
	const std::string dataFile = "datamemory.txt";
	const std::string instructionFile = "instructions.txt";
	const std::string simulationOutputFile = "simulation.txt";
	int t{};

	M_Sim petrinetSimulator(instructionFile, regFile, dataFile, simulationOutputFile);

	petrinetSimulator.runSimulation();
	petrinetSimulator.writeReportToFile();

	std::cout << petrinetSimulator.showReport();
	std::cin >> t;

	return 0;
}