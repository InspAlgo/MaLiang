#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <iostream>

typedef struct
{
    char character;
    int weight;
    int parent, left_child, right_child;
}HTNode, *HuffmanTree;

typedef char **HuffmanCode;
typedef char *EachHuffcode;

class Huff
{
public:
    EachHuffcode *pbm;

    // 找出权值最小的节点或子树
    void Select(HuffmanTree HT, int length, int &s1, int &s2)
    {
        unsigned int min = 0xffffffff;  // 给 min 赋初值（足够大）
        HuffmanTree p = HT;
        int i;

        for(i = 1, ++p; i <= length; ++i, ++p)
        {
            if(p->weight < min && p->parent == 0)
            {
                min = p->weight;
                s1 = i;
            }
        }

        min = 0xffffffff;  // 恢复 min 的初值，重新开始计数找出权值第二小的节点或者子树
        p = HT;

        for(i = 1, ++p; i <= length; ++i, ++p)
        {
            if(p->weight < min && p->parent == 0 && i != s1)
            {
                min = p->weight;
                s2 = i;
            }
        }
    }

    void HuffmanCoding(HuffmanTree &HT, HuffmanCode &HC, int *w, int n)
    {
        if(n < 1)
            return;

        int m = 2*n - 1;  // Huffman树的节点数
        HT = new HTNode[m+1];  // 多定义一位，保证第一位空置不用
        HuffmanTree p = HT;
        int i, s1, s2, c, f, start;

        for(i = 1, ++p; i <= n; ++i, ++p, ++w)
        {
            p->weight = *w;
            p->parent = p->left_child = p->right_child = 0;
        }

        for(;i <= m; ++i, ++p)  // 把后面的节点都置 0
            p->weight = p->parent = p->left_child = p->right_child = 0;

        for(i = n + 1; i <= m; i++)  // 构建 Huffman 树
        {
            this->Select(HT, i-1, s1, s2);
            HT[s1].parent = i;
            HT[s2].parent = i;
            HT[i].left_child = s1;
            HT[i].right_child = s2;
            HT[i].weight = HT[s1].weight + HT[s2].weight;
        }

        HC = new EachHuffcode[n+1];  // 多定义一位，保证第一位空置不用
        char *cd = new char[n];
        cd[n-1] = '\0';  // 末位的标志，使传值时能到此停止

        // 得到各节点的霍夫曼编码，并将他们记录在HC[1]到HC[n]中
        for(i = 1; i <= n; ++i)
        {
            start = n - 1;
            for(c = i, f = HT[i].parent; f != 0; c = f, f = HT[f].parent)
            {
                if(c == HT[f].left_child)
                    cd[--start] = '0';
                else
                    cd[--start] = '1';

                HC[i] = new char[n-start];  // 给 HC[] 分配储存空间
                strcpy(HC[i], &cd[start]);  // 将 cd[] 所储存的 Huffman 编码传给 HC[][]
            }
        }

        delete[] cd;  // 释放空间
    }

    Huff(int x, int *weight)
    {
        HuffmanTree hf_tree = NULL;
        HuffmanCode hf_code = NULL;

        HuffmanCoding(hf_tree, hf_code, weight, x);
        this->pbm = hf_code;
        ++pbm;
    }
};

#endif // HUFFMAN_TREE_H
