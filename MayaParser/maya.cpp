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

	// �� ��� �б�
	GetModelPath(path);

	// ����, �ؽ���, �븻, ���̽� ���� �б�
	result = ReadFileCounts(path, vertexCount, textureCount, normalCount, faceCount);
	if (!result) return -1;

	// ���� ȭ�鿡 ����
	cout << endl;
	cout << "Vertices : " << vertexCount << endl;
	cout << "UVs      : " << textureCount << endl;
	cout << "Normals  : " << normalCount << endl;
	cout << "Faces    : " << faceCount << endl;

	// ���Ϸκ��� ������ �б�
	result = LoadDataStructures(path, vertexCount, textureCount, normalCount, faceCount);
	if (!result) return -1;

	// ���� ��ȯ ����
	cout << "\nFile has been converted.\nDo you wish to exit(y/n)?";
	cin >> garbage;
	return 0;
}

void GetModelPath(char* path)
{
	bool done;
	ifstream fin;

	// ���� ���� �� ���� ����
	done = false;
	while (!done)
	{
		// ���� �̸� �ۼ�
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

	// �ʱ�ȭ
	vertexCount = textureCount = normalCount = faceCount = 0;

	// ���� ����
	fin.open(path);
	if (fin.fail()) return false;

	// ���� ������ �б�
	fin.get(input);
	while (!fin.eof())
	{
		// �� ���� ������ v�� ��� �� �ڿ� ���� �����Ŀ� ���� �ൿ �ٸ���
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') ++vertexCount;
			else if (input == 't') ++textureCount;
			else if (input == 'n') ++normalCount;
		}

		// �� ���� ������ f�� ��� faceCount ����
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') ++faceCount;
		}

		// �������� �ǳ� �ٱ�
		while (input != '\n' && !fin.eof())
			fin.get(input);

		// ���� �� �б�
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

	// �� ���� ����ü �ʱ�ȭ
	vertices = new VertexType[vertexCount];
	if (!vertices) return false;

	texcoords = new VertexType[textureCount];
	if (!texcoords) return false;

	normals = new VertexType[normalCount];
	if (!normals) return false;

	faces = new FaceType[faceCount];
	if (!faces) return false;

	// �ε��� �ʱ�ȭ
	vertexIndex = texcoordIndex = normalIndex = faceIndex = faceVertexCount = 0;

	// ���� ����
	fin.open(path);
	if (fin.fail()) return false;

	// ����, �ؽ��� ��ǥ, ���� �о ����ü�� �ֱ�
	// ���� �ؾ� �� ���� ���ߴ� ������ ��ǥ���̱� ������ �޼� ��ǥ��� ��ȯ����� �Ѵٴ� ��.
	fin.get(input);
	while (!fin.eof())
	{
		// ���� ���� �б�
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

		// �� �б�
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				
				// �������� �簢���� ���
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

		// �ʿ� ���� �κ��� �ѱ��
		while (input != '\n' && !fin.eof())
			fin.get(input);

		// ���� �� �����ϱ� 
		fin.get(input);
	}

	// ���� �ݱ�
	fin.close();

	// ����� ���� ����
	fout.open("model.txt");

	// ���� ���(���� ���� �ۼ�)
	fout << "Vertex Count: " << (faceVertexCount) << endl;
	fout << endl;
	fout << "Data: " << endl;
	fout << endl;

	// �� �鿡 ���� �� ���� ������ �����.
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

		// �簢���� ���
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

	// �Ҵ� ����
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