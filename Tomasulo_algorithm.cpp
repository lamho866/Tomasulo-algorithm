#include "bits/stdc++.h"

using namespace std;

struct Inst
{
	char opCode[5];
	int r1, r2, r3;
};

struct RS_type
{
	int notation;
	int rs, r1, r2;
	bool r1Exist = false, r2Exist = false;
	bool ready = false;
};

class RAT
{
public:
	int rat[32];
	int reg[32];
	vector<int> ROB;
	vector<int> regROB;// this ROS is write which ratister result
	vector<bool> done;
	bool dirty[32];// true dirty, false no dirty
	size_t comit;
	size_t PC;
	size_t comitLimit;
	size_t ratSize = 0;
	size_t instNum;

	RAT(size_t ratsize, size_t instSize, size_t comitNum) :
		comit(0), PC(0), comitLimit(comitNum), ratSize(ratsize), instNum(instSize)
	{
		regROB.resize(instSize);
		ROB.resize(instSize);
		done.resize(instSize, false);
		memset(dirty, 0, sizeof(dirty));
		memset(rat, 0, sizeof(rat));
		memset(reg, 0, sizeof(reg));
	}

	void regInitial()
	{
		for (int i = 1; i <= ratSize; ++i)
			cin >> reg[i];
	}

	void comitWork()
	{
		for (int i = 0; i < comitLimit; ++i)
			if (done[comit])
				comit++;
			else
				break;
	}

	RS_type Decoder(Inst& inst)
	{
		RS_type RS;
	
		if (!strcmp(inst.opCode,"ADDI"))
			RS.notation = 0, RS.r2Exist = true;
		else if (!strcmp(inst.opCode, "ADD"))
			RS.notation = 1;
		else if (!strcmp(inst.opCode,"SUB"))
			RS.notation = 2;
		else if (!strcmp(inst.opCode,"MUL"))
			RS.notation = 3;
		else if (!strcmp(inst.opCode, "DIV"))
			RS.notation = 4;
		
		RS.rs = inst.r1;
		if (dirty[inst.r2])
			RS.r1 = rat[inst.r2], RS.r1Exist = false;
		else
			RS.r1 = reg[inst.r2], RS.r1Exist = true;
	
		if(RS.notation == 0)
			RS.r2 = inst.r3;
		else
			if (dirty[inst.r3])
				RS.r2 = rat[inst.r3], RS.r2Exist = false;
			else
				RS.r2 = reg[inst.r3], RS.r2Exist = true;

		RS.ready = false;
		
		if (RS.rs == RS.r1 && !dirty[RS.rs])
			RS.r1Exist = true;

		if (RS.rs == RS.r2 && !dirty[RS.rs])
			RS.r2Exist = true;

		regROB[PC] = inst.r1;
		RS.rs = PC;
		rat[inst.r1] = PC++;
		dirty[inst.r1] = true;

		return RS;
	}
	
	void outPut()
	{
		cout << setw(6) << '_' << "RF_______\n";
		for (int i = 1; i <= ratSize; ++i)
		{
			string regName = 'F' + to_string(i);
			cout << setw(6) << 'F' << regName << '|' << setw(5) << reg[i] << '|' << endl;
		}

		cout << setw(6) << '-' << "------" << endl;

		cout << endl;

		cout << setw(6) << '_' << "RAT_______\n";
		for (int i = 1; i <= ratSize; ++i)
		{
			string ratName = 'F' + to_string(i);
			if (dirty[i])
			{
				string robName = "ROB" + to_string(rat[i]);
				cout << setw(6) << 'F' << ratName << '|' << setw(5) << robName << '|' << endl;
			}
			else
			{
				cout << setw(6) << 'F' << ratName << '|' << setw(5) << ' ' << '|' << endl;
			}
		}
		cout << setw(6) << '-' << "------" << endl;
		cout << endl;
	}

	bool all_inst_done()
	{
		return comit >= instNum;
	}
};

vector<Inst> instOrder;
size_t readPC = 0;
// ADDI ADD SUB MUL DIV
char symbol[5] = { '+', '+' , '-', '*', '/' };
int cycleLimie[5] = { 0,0,0,0,0 };


void readInst(ifstream &file)
{
	Inst buf;
	char notNeed;
	
	while (file >> buf.opCode)
	{
		file >> notNeed >> buf.r1 >> notNeed;
		file >> notNeed >> buf.r2 >> notNeed;
		if (strcmp(buf.opCode,"ADDI"))
			file >> notNeed;
		file >> buf.r3;

		instOrder.push_back(buf);
	}
}

class RS_Dispatch
{
public:
	RS_Dispatch(size_t RSsize, RAT* ratPtr) :RS_size(RSsize), RATptr(ratPtr), dispatch_used(false)
	{
		RS.resize(RSsize);
		usedRS.resize(RSsize, false);
		RS_usedSize = 0;
	}

	bool RS_full()
	{
		return RS_usedSize >= RS_size;
	}

	void RS_pushIn(RS_type &RSdata)
	{
		for(int i = 0; i < RS_size; ++i)
			if (!usedRS[i])
			{
				RS[i] = RSdata;

				usedRS[i] = true;
				++RS_usedSize;
				break;
			}
	}

	void RS_dispatch()
	{
		if (!dispatch_used && !dispatch_order.empty())
		{
			dispatch_used = true;
			cur_cycle = 0;
			dispatch_index = dispatch_order.front();
			dispatch_order.pop();
		}
		else
			cur_cycle++;
			
	}

	bool canWB()
	{
		if (dispatch_used && cur_cycle >= cycleLimie[RS[dispatch_index].notation])
			return true;

		return false;
	}

	void writeBack()
	{
		if (RS[dispatch_index].notation == 4 && RS[dispatch_index].r2 == 0)
		{
			printf("Expection Occurs : divide by zero\n");
			system("pause");
			exit(0);
		}
		int result;
		switch (symbol[RS[dispatch_index].notation])
		{
		case '+':
			result = RS[dispatch_index].r1 + RS[dispatch_index].r2;
			break;
		case '-':
			result = RS[dispatch_index].r1 - RS[dispatch_index].r2;
			break;
		case '*':
			result = RS[dispatch_index].r1 * RS[dispatch_index].r2;
			break;
		case '/':
			result = RS[dispatch_index].r1 / RS[dispatch_index].r2;
			break;
		default:
			break;
		}
		int rsROBIndex = RS[dispatch_index].rs;
		int rsRegIndex = RATptr->regROB[rsROBIndex];

		--RS_usedSize;
		usedRS[dispatch_index] = false;

		RATptr->ROB[rsROBIndex] = result;
		RATptr->done[rsROBIndex] = true;
		RATptr->reg[rsRegIndex] = result;
		
		if (RATptr->rat[rsRegIndex] == rsROBIndex)
		{
			RATptr->rat[rsRegIndex] = result;
			RATptr->dirty[rsRegIndex] = false;
		}


	}

	void upData()
	{
		for (int i = 0; i < RS_size; ++i)
		{
			if (usedRS[i] && RS[i].ready == false)
			{
				if (!RS[i].r1Exist && RATptr->done[RS[i].r1])
				{
					RS[i].r1Exist = true;
					RS[i].r1 = RATptr->ROB[RS[i].r1];
				}
				if (!RS[i].r2Exist && RATptr->done[RS[i].r2])
				{
					RS[i].r2Exist = true;
					RS[i].r2 = RATptr->ROB[RS[i].r2];
				}

				RS[i].ready = RS[i].r1Exist && RS[i].r2Exist;
				if(RS[i].ready)
					dispatch_order.push(i);
			}
		}
	}

	void outPut()
	{
		
		for (int i = 0; i < RS_size; ++i)
		{
			string rsName = "RS" + to_string(i + 1);
			if (usedRS[i])
			{
				string rs1Name = "ROB" + to_string(RS[i].r1);
				string rs2Name = "ROB" + to_string(RS[i].r2);
				cout << setw(6) << rsName << '|' << setw(5) << symbol[RS[i].notation] << '|';

				cout << setw(5);
				if (RS[i].r1Exist)
					cout << RS[i].r1 << '|';
				else
					cout << rs1Name << '|';

				cout << setw(5);
				if (RS[i].r2Exist)
					cout << RS[i].r2 << '|';
				else
					cout << rs2Name << '|';
			}
			else
				cout << setw(6) << rsName << '|' << setw(5) << ' ' << '|' << setw(5) << ' ' << '|' << setw(5) << ' ' << '|';

			cout << endl;
		}

		cout << "    ----------------------\n";
		if (dispatch_used)
			printf("BUFFER: (RS%d) %d %c %d\n",
				dispatch_index + 1, RS[dispatch_index].r1, symbol[RS[dispatch_index].notation], RS[dispatch_index].r2);
		else
			printf("BUFFER:            \n");
		cout << endl;

	}

	RAT * RATptr;
	size_t RS_size;
	size_t RS_usedSize;
	queue<int> dispatch_order;
	vector<RS_type> RS;
	vector<bool> usedRS;

	int dispatch_index;
	bool dispatch_used;
	int cur_cycle;
};

class WriteBack
{
public:
	WriteBack(RS_Dispatch* addPtr, RS_Dispatch* mulPtr, RAT *ratPtr, bool priorityBit = 0) :
		addRSptr(addPtr), mulRSptr(mulPtr), RATptr(ratPtr), priority(priorityBit){}

	void WB()
	{
		if (addRSptr->canWB() && mulRSptr->canWB())
		{
			if (priority == 0)
			{
				addRSptr->dispatch_used = false;
				addRSptr->writeBack();
			}
			else
			{
				mulRSptr->dispatch_used = false;
				mulRSptr->writeBack();
			}
		}
		else if (addRSptr->canWB())
		{
			addRSptr->dispatch_used = false;
			addRSptr->writeBack();
		}
		else if (mulRSptr->canWB())
		{
			mulRSptr->dispatch_used = false;
			mulRSptr->writeBack();
		}


		addRSptr->upData();
		mulRSptr->upData();

		if(addRSptr->dispatch_used == false)
			addRSptr->RS_dispatch();
		if (mulRSptr->dispatch_used == false)
		mulRSptr->RS_dispatch();

		RATptr->comitWork();
	}

	RS_Dispatch *addRSptr, *mulRSptr;
	RAT *RATptr;
	bool priority;// 0 is add > mul, 1 is mul > add
};

int main()
{
	int CycleNum = 0;
	int regSize = 5;
	int addRS_size = 3, mulRS_size = 2;


	cycleLimie[0] = 1; //ADDI
	cycleLimie[1] = 5; //ADD
	cycleLimie[2] = 1; //SUB
	cycleLimie[3] = 1; //MUL
	cycleLimie[4] = 1; //DIV

	ifstream inFile("inFile.txt");
	readInst(inFile);
	inFile.close();

	RAT ratBuffer(regSize, instOrder.size(), 1);// register size, inst size, commit num per evey times

	//ratBuffer.regInitial();

	RS_Dispatch addRS(addRS_size, &ratBuffer);
	RS_Dispatch mulRS(mulRS_size, &ratBuffer);
	WriteBack WB(&addRS, &mulRS, &ratBuffer, 0);

	RS_type nextInstBuf;
	bool pushIn = true;

	while (!ratBuffer.all_inst_done())
	{
		printf("Cycle %d\n\n", ++CycleNum);
		ratBuffer.reg[0] = 0;
		addRS.upData();
		mulRS.upData();

		if (pushIn && readPC < instOrder.size())
			nextInstBuf = ratBuffer.Decoder(instOrder[readPC]);

		if (readPC < instOrder.size()) //avoid all inst is readed
		{
			if (nextInstBuf.notation <= 2 && !addRS.RS_full()) // 0 1 2
			{
				addRS.RS_pushIn(nextInstBuf);
				pushIn = true;
				readPC++;
			}
			else if (nextInstBuf.notation > 2 && !mulRS.RS_full())
			{
				mulRS.RS_pushIn(nextInstBuf);
				pushIn = true;
				readPC++;
			}
			else
				pushIn = false;
		}

		addRS.RS_dispatch();
		mulRS.RS_dispatch();
		

		ratBuffer.outPut();
		cout << setw(6) << "___" << "RS________________\n";
		addRS.outPut();
		cout << setw(6) << "___" << "__________________\n";
		mulRS.outPut();

		WB.WB();
	}

	printf("Cycle %d\n\n", ++CycleNum);
	addRS.RS_dispatch();
	mulRS.RS_dispatch();

	ratBuffer.outPut();
	cout << setw(6) << "___" << "RS________________\n";
	addRS.outPut();
	cout << setw(6) << "___" << "__________________\n";
	mulRS.outPut();


	//system("pause");
	return 0;
}