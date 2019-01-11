class Process
{
public:
	Process();
	~Process();
	void CreateProcessThread(std::string _commando);
	int GetProcessAmount();
private:
	int processAmount = 0;
	std::vector<PROCESS_INFORMATION> processList;
};

