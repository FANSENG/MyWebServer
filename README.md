# MyWebServer
C++ WebServer 实战项目。

## TODO
- sqlpool

## 实现步骤

1. 实现 互斥锁、条件变量、信号量
2. 实现阻塞队列
3. 实现 MySQL 连接池
4. 实现线程池
5. 实现 http 请求部分
6. 实现 Timer 计时器

## 优化
1. 互斥锁能否优化为读写锁
2. 信号量 中 增加限时wait的操作，在获取数据库连接时使用
3. 之前项目中使用的 条件变量只用来构成阻塞，单纯的锁也可以达到这个效果，可以想办法优化此处，要引入**条件**。

## 文件说明
- src/ 项目源码
- test/ 测试代码
- build/ 编译后的可执行文件
- src/config/ 配置文件
- src/log/ 日志文件
- src/http/ http 连接
- src/lock/ 互斥锁、条件变量、信号量
- src/threadpool/ 线程池
- src/timer 时间计时器
- src/utils 工具类
- src/webserver 服务器