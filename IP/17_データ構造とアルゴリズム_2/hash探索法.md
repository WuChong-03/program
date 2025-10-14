## ハッシュ探索法 (Hash Search)

ハッシュ探索法 是一种利用 `hash 函数` 来决定 `key` 在 ハッシュ表`hash table` 中存放位置 `index` 的搜索方法。

这种方法的优点是：

- 1️⃣ **可计算输入无限制**

    不论输入多大、多复杂（数字、文字、文件名）都行，
    输入空间是“无限的”，但输出空间是“固定的”。

- 2️⃣ **只需计算一次即可定位**

    哈希函数不需要像线性搜索那样一个个比，也不用像二分那样折半查  找。只要算一次

cpp例:
```c
    int data[] = {532, 463, 292};   // 想要存入的数据
    int table[10] = {0};            // 大小为10的哈希表
    int index;

    for (int i = 0; i < 3; i++) {
        index = data[i] % 10;       // 哈希函数例：取余运算
        printf("数据 %d 存放在table[%d]\n", data[i], index);
        table[index] = data[i];     // 存入对应位置
    }
```

## 关于冲突 (collision)

在上面例子中，532 和 292 通过取余运算得到相同的结果，因此被存入同一个格子。
这种情况称为 「**冲突　Collision / コリジョン**」

例:

| 数据  | 计算结果                 | 存放位置       |
| --- | -------------------- | ---------- |
| 532 | (532 mod 10) + 1 = 3 | table[3]       |
| 463 | (463 mod 10) + 1 = 4 | table[4]       |
| 292 | (292 mod 10) + 1 = 3 | table[3]（❗冲突） |

—— 因冲突而无法存入的记录，称为 **同义记录（シノニムレコード / synonym record**

## 关于考试

在基本情报管理考试中, hash探索的题目多数涉及两个方面:

 - hash探索的特性
 - 数据在hash表的存放位置
 - 根据试题要求的冲突发生时的处理办法再存放
    
详见: 课题17_1
https://docs.google.com/forms/d/e/1FAIpQLSdfuVxr5_FkmHhZXopWiQaQes7divvbgJ1g5gmXjr0UN_kspA/viewscore?viewscore=AE0zAgCLAVyRECHaOMyuo5keSnnanQSO1jSEy51nqxFfQ21p_hfNzdFnwq_Atzd7Ng