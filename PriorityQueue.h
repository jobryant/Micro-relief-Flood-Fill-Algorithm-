#include <iostream>
using namespace std;

struct GPoint
{
	int row;
	int col;
	float Value;
	GPoint* left;
	GPoint* right;
};


class PriorityQueue
{
public:
	void Push(GPoint* pt)
	{
		if (pt->Value)
		{
		}
	}

	GPoint Pop()
	{
		
		
	}

private:
	GPoint*	queue;
};
