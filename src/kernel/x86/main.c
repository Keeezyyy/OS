

#define VRAM 0xB8000

__attribute__((used)) int main()
{
    for (int i = 0; i < 1000; i++)
    {
        *((char *)VRAM + i) = 0;
    }
    while (1)
    {
    }
}