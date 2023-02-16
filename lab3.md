对于一个在队列中的文件，定义它的参数信息：

1. edge_cnt：覆盖到的不同边数
1. cover_cnt：覆盖到的所有边的次数
1. bonus：变异有效率

如果利用 target 生成了一个有新覆盖的变异 newone

我们定义 newone 的效率是：

(newone->edge_cnt - target->edge_cnt)+(1/3)(newone->cover_cnt - target->cover_cnt)

把这个效率加到 newone 的祖先版本的 bonus 参数里，衰减率为 DE_RATE = *(3/4)



挑选一个状态：首先获取所有状态的价值 = edge_cnt+cover_cnt+bonus

然后随机化退火 （状态数） 次



30~33字节：biCompression

