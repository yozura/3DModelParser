#include <iostream>
#include <fstream>
using namespace std;

typedef struct
{
	float x, y, z;
}VertexType;

typedef struct
{
	int vIndex0, vIndex1, vIndex2, vIndex3;
	int tIndex0, tIndex1, tIndex2, tIndex3;
	int nIndex0, nIndex1, nIndex2, nIndex3;
}FaceType;

void GetModelPath(char*);
bool ReadFileCounts(char*, int&, int&, int&, int&);
bool LoadDataStructures(char*, int, int, int, int);

int main()
{
	bool result;
	char path[256];
	int vertexCount, textureCount, normalCount, faceCount;
	char garbage;

	// 모델 경로 읽기
	GetModelPath(path);

	// 정점, 텍스쳐, 노말, 페이스 개수 읽기
	result = ReadFileCounts(path, vertexCount, textureCount, normalCount, faceCount);
	if (!result) return -1;

	// 개수 화면에 띄우기
	cout << endl;
	cout << "Vertices : " << vertexCount << endl;
	cout << "UVs      : " << textureCount << endl;
	cout << "Normals  : " << normalCount << endl;
	cout << "Faces    : " << faceCount << endl;

	// 파일로부터 데이터 읽기
	result = LoadDataStructures(path, vertexCount, textureCount, normalCount, faceCount);
	if (!result) return -1;

	// 파일 변환 통지
	cout << "\nFile has been converted.\nDo you wish to exit(y/n)?";
	cin >> garbage;
	return 0;
}

void GetModelPath(char* path)
{
	bool done;
	ifstream fin;

	// 파일 얻을 때 까지 루프
	done = false;
	while (!done)
	{
		// 파일 이름 작성
		cout << "Enter model filename: ";
		cin >> path;

		fin.open(path);
		if (fin.good())
		{
			done = true;
		}
		else
		{
			fin.clear();
			cout << endl;
			cout << "File " << path << " could not be opened" << endl << endl;
		}
	}
}

bool ReadFileCounts(char* path, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fin;
	char input;

	// 초기화
	vertexCount = textureCount = normalCount = faceCount = 0;

	// 파일 열기
	fin.open(path);
	if (fin.fail()) return false;

	// 파일 끝까지 읽기
	fin.get(input);
	while (!fin.eof())
	{
		// 한 줄의 시작이 v일 경우 그 뒤에 뭐가 오느냐에 따라 행동 다르게
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') ++vertexCount;
			else if (input == 't') ++textureCount;
			else if (input == 'n') ++normalCount;
		}

		// 한 줄의 시작이 f일 경우 faceCount 증가
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') ++faceCount;
		}

		// 나머지는 건너 뛰기
		while (input != '\n' && !fin.eof())
			fin.get(input);

		// 다음 줄 읽기
		fin.get(input);
	}

	fin.close();
	return true;
}

bool LoadDataStructures(char* path, int vertexCount, int textureCount, int normalCount, int faceCount)
{
	VertexType* vertices, * texcoords, * normals;
	FaceType* faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;
	int faceVertexCount;

	// 네 개의 구조체 초기화
	vertices = new VertexType[vertexCount];
	if (!vertices) return false;

	texcoords = new VertexType[textureCount];
	if (!texcoords) return false;

	normals = new VertexType[normalCount];
	if (!normals) return false;

	faces = new FaceType[faceCount];
	if (!faces) return false;

	// 인덱스 초기화
	vertexIndex = texcoordIndex = normalIndex = faceIndex = faceVertexCount = 0;

	// 파일 열기
	fin.open(path);
	if (fin.fail()) return false;

	// 정점, 텍스쳐 좌표, 법선 읽어서 구조체에 넣기
	// 주의 해야 할 점은 마야는 오른손 좌표계이기 때문에 왼손 좌표계로 변환해줘야 한다는 것.
	fin.get(input);
	while (!fin.eof())
	{
		// 정점 정보 읽기
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;
				vertices[vertexIndex].z *= -1.0f;
				++vertexIndex;
			}
			else if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y >> texcoords[texcoordIndex].z;
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				++texcoordIndex;
			}
			else if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;
				normals[normalIndex].z *= -1.0f;
				++normalIndex;
			}
		}

		// 면 읽기
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				
				// 폴리곤이 사각형인 경우
				fin.get(input);
				if (input == ' ')
				{
					fin.get(input);
					if (input != '\n')
					{
						fin >> faces[faceIndex].vIndex0 >> input2 >> faces[faceIndex].tIndex0 >> input2 >> faces[faceIndex].nIndex0;
						faceVertexCount += 6;
					}
					else
					{
						faces[faceIndex].vIndex0 = 0;
						faces[faceIndex].tIndex0 = 0;
						faces[faceIndex].nIndex0 = 0;
						faceVertexCount += 3;
					}
				}
				else
				{
					faces[faceIndex].vIndex0 = 0;
					faces[faceIndex].tIndex0 = 0;
					faces[faceIndex].nIndex0 = 0;
					faceVertexCount += 3;
				}
				++faceIndex;
			}
		}

		// 필요 없는 부분은 넘기기
		while (input != '\n' && !fin.eof())
			fin.get(input);

		// 다음 줄 시작하기 
		fin.get(input);
	}

	// 파일 닫기
	fin.close();

	// 출력할 파일 열기
	fout.open("model.txt");

	// 파일 헤더(정점 개수 작성)
	fout << "Vertex Count: " << (faceVertexCount) << endl;
	fout << endl;
	fout << "Data: " << endl;
	fout << endl;

	// 각 면에 대해 세 개의 정점을 출력함.
	for (int i = 0; i < faceIndex; ++i)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		// 사각형인 경우
		if (faces[i].vIndex0 != 0)
		{
			vIndex = faces[i].vIndex1 - 1;
			tIndex = faces[i].tIndex1 - 1;
			nIndex = faces[i].nIndex1 - 1;

			fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
				<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
				<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

			vIndex = faces[i].vIndex3 - 1;
			tIndex = faces[i].tIndex3 - 1;
			nIndex = faces[i].nIndex3 - 1;

			fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
				<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
				<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

			vIndex = faces[i].vIndex0 - 1;
			tIndex = faces[i].tIndex0 - 1;
			nIndex = faces[i].nIndex0 - 1;

			fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
				<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
				<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		}
	}

	fin.close();

	// 할당 해제
	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}

	if (texcoords)
	{
		delete[] texcoords;
		texcoords = 0;
	}

	if (normals)
	{
		delete[] normals;
		normals = 0;
	}

	if (faces)
	{
		delete[] faces;
		faces = 0;
	}

	return true;
}