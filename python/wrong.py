# encoding: utf-8
# 函数默认值是dict，一旦不保持向里传参，过程中又可能被修改，就可能是一个灾难; 会有一个属于函数默认值的分配空间
def _try_diff_key_map(aa, diff_key_map={}):
    diff_key_map.update(aa)
    return diff_key_map

_try_diff_key_map({'a':3})
_try_diff_key_map({'b':3})

def _try_diff_key_map(aa, diff_key_map=None):
    diff_key_map = diff_key_map or {}
    diff_key_map.update(aa)
    return diff_key_map
# 大部分时候是个安全写法，但是当 diff_key_map 本身要被递归更新的时候也许是个危险主意

# 中间过程分配的变量就不在函数的堆上了，而是有自己的中间空间

aa = {}
b = aa or {}
b.update({'a':3})
# 这种情况下，aa是不被更新的
# 在需要被更新的时候写的更保守： diff_key_map = diff_key_map if diff_key_map is not None else {}

# 同样的问题出在下面这种情形下
def _create_diff_key_map(node_path, diff_keys, diff_key_map=None):
    diff_key_map = None or {}
    if len(node_path) == 1:
        key = node_path[0]
        diff_key_map[key] = diff_keys
    else:
        key = node_path[0]
        diff_key_map.update({key:{}})
        _create_diff_key_map(node_path[1:], diff_keys=diff_keys, diff_key_map=diff_key_map[key])

    return diff_key_map

def _create_diff_key_map(node_path, diff_keys, diff_key_map={}):

    if len(node_path) == 1:
        key = node_path[0]
        diff_key_map[key] = diff_keys
    else:
        key = node_path[0]
        diff_key_map.update({key:{}})
        _create_diff_key_map(node_path[1:], diff_keys=diff_keys, diff_key_map=diff_key_map[key])

    return diff_key_map


# 这类多层的递归不应该直接用 return
### 命名空间不重名是关键么？
## 不是，不主动传的话就会一直是原始的那个 {}，不重新分配空间，所以直接在原map上更新了
    
def _create_diff_key_map(node_path, diff_keys, diff_key_map={}):
    if len(node_path) == 1:
        print node_path
        key = node_path[0]
        print diff_key_map
        diff_key_map[key] = diff_keys
        return diff_key_map
    else:
        key = node_path[0]
        diff_key_map[key] = {}
        print diff_key_map[key]
        return _create_diff_key_map(node_path[1:], diff_key_map[key])

## 改为这种：会直接一路return回去
def _create_diff_key_map(node_path, diff_keys, diff_key_map={}):
    if len(node_path) == 1:
        print node_path
        key = node_path[0]
        print 'see when1', diff_key_map
        diff_key_map[key] = diff_keys
        return diff_key_map
    else:
        key = node_path[0]
        diff_key_map.update({key:{}})
        print 'see', diff_key_map[key]
        aa = _create_diff_key_map(node_path[1:], diff_keys, diff_key_map[key])
        print aa
        return aa

## 因此需要一路修改，最终return
