# encoding: utf-8

## 以下会使得emb里的每个list都被更新
emb = [[]] * 128
emb[0].append(1)

## 正确写法
emb.append([])
emb.append([])