class ChunkCoord
{
public:
	// Default constructor
	ChunkCoord() : X(0), Y(0) {}

	// Constructor with parameters
	ChunkCoord(int X, int Y);
	ChunkCoord(FVector Pos);

	~ChunkCoord();

	bool Equals(const ChunkCoord& Other);

	int X;
	int Y;
};
