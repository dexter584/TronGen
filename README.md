# TRX/波场地址靓号生成器

基于 OpenCL 的 TRON 靓号地址生成工具（改自 ethereum profanity）。仓库包含源码、Visual Studio 工程，以及从源码编译所需的 OpenCL/OpenSSL 头文件与导入库。

亮点：

- 完全开源，核心生成、匹配与输出逻辑都可审查。
- 绿色免安装，Windows 版本下载解压后即可运行。
- 支持自行编译，仓库包含 Visual Studio 工程与必要的 OpenCL/OpenSSL 编译文件。
- 基于 OpenCL，面向 NVIDIA / AMD 等 GPU 设备；具备跨平台移植基础。
- 使用 GPU 批量生成与筛选地址，适合长时间高效搜索靓号后缀。
- CPU 运行需要 OpenCL CPU runtime 与对应设备枚举支持；当前 v1.0.0 发布包默认面向 OpenCL GPU。

程序会批量生成 TRON 私钥与地址，并按“前缀/后缀匹配”规则筛选。命中后输出格式为：

```text
private_key_hex,TRON_address
```

强烈建议：不要把生成结果公开、上传到仓库或发给任何人；私钥一旦泄露，对应地址资产可能被转走。

---

## 1. 快速开始（普通使用）

下载已编译好的 Windows OpenCL 版本：

[Release v1.0.0 下载页面](https://github.com/dexter584/TronGen/releases/tag/v1.0.0)

下载后解压，解压后的目录结构大致为：

```text
TRX-Tron-vanity-address-generator-windows-opencl\
  profanity.exe
  OpenCL.dll
  libcrypto-3-x64.dll
  matching\
  scripts\
```

### 1.1 启动脚本位置

启动脚本在解压目录的 `scripts\` 目录里。双击运行对应脚本即可开始生成：

```text
scripts\suffix_digits_6.bat
scripts\suffix_digits_7.bat
scripts\suffix_digits_8.bat
scripts\suffix_digits_9.bat
scripts\suffix_letters_6.bat
scripts\suffix_letters_7.bat
scripts\suffix_letters_8.bat
scripts\suffix_letters_9.bat
```

脚本名称含义：

- `suffix_digits_6.bat`：生成后缀匹配数字规则、后缀至少 6 位命中的地址。
- `suffix_digits_7.bat` / `suffix_digits_8.bat` / `suffix_digits_9.bat`：数字后缀位数分别为 7、8、9。
- `suffix_letters_6.bat` / `suffix_letters_7.bat` / `suffix_letters_8.bat` / `suffix_letters_9.bat`：字母后缀位数分别为 6、7、8、9。

### 1.2 生成结果位置

脚本运行后会在解压目录下自动创建 `result\` 目录，命中的结果写入：

```text
result\*.txt
```

例如运行：

```text
scripts\suffix_digits_6.bat
```

默认会生成：

```text
result\suffix_digits_6.txt
```

结果文件每行格式为：

```text
private_key_hex,TRON_address
```

### 1.3 运行缓存位置

OpenCL kernel 编译缓存会自动写入解压目录下的：

```text
cache\
```

`cache\` 可以删除；下次运行时程序会自动重新生成。

---

## 2. 命令行使用说明（详细）

建议在 `dist\` 目录下运行（方便相对路径与缓存目录）：

```powershell
cd dist
.\profanity.exe --help
```

### 2.1 核心参数（常用）

- `--matching <path|address>`：匹配输入。可以是“匹配文件路径”，也可以是“单个 TRON 地址”。
- `--prefix-count <0..10>`：要求命中的最小前缀字符数（默认 `0`）。
- `--suffix-count <0..10>`：要求命中的最小后缀字符数（默认 `6`）。
- `--quit-count <N>`：命中结果累计达到 N 后退出（默认 `0` 表示不按数量退出）。
- `--skip <index>`：跳过指定索引的设备（可重复传入多次）。
- `--output <path>`：将命中结果追加写入到文件（格式：`private,address` 每行一条）。
- `--no-cache`：禁用 OpenCL 编译缓存（默认启用；启用时会在运行目录创建/使用 `cache\`）。

### 2.2 匹配文件格式（非常重要）

匹配规则只关注地址的“前 10 个字符 + 后 10 个字符”，中间 14 个字符会被忽略（这也是 `prefix-count` / `suffix-count` 最大为 10 的原因）。

匹配文件支持两种行格式（每行一条，忽略其它长度的行）：

1) **34 字符的完整 TRON 地址**（通常以 `T` 开头）

```text
TUqEg3dzVEJNQSVW2HY98z5X8SBdhmao8D
```

2) **20 字符的“前10+后10”拼接串**（手工拼接：`地址前10位 + 地址后10位`）

```text
TUqEg3dzVEdhmao8D
```

你可以把想要的靓号“前缀/后缀目标”写成上述任一格式，程序会把它转换为内部匹配规则。

### 2.3 常见用法示例

按匹配文件批量跑，找到 200 条就退出，并把结果写入文件：

```powershell
.\profanity.exe --matching matching\digits.txt --prefix-count 0 --suffix-count 6 --quit-count 200 --output result\suffix_digits_6.txt
```

只跑单个地址（例如要求前缀至少 2 位、后缀至少 4 位，找到 1 条就退出）：

```powershell
.\profanity.exe --matching TUqEg3dzVEJNQSVW2HY98z5X8SBdhmao8D --prefix-count 2 --suffix-count 4 --quit-count 1 --output result\one.txt
```

跳过第 1 号设备（设备列表会在程序启动时打印）：

```powershell
.\profanity.exe --matching matching\digits.txt --skip 1 --output result\skip_gpu1.txt
```

### 2.4 高级参数（性能/兼容性调参）

以下参数属于“二开/深度调参”范畴，默认值通常就够用：

- `--work <size>`（默认 `64`）：OpenCL local work-group size。某些设备/驱动不支持指定值时，程序会自动回退为“让 OpenCL 自己决定”。
- `--work-max <size>`（默认 `0`）：限制单次 enqueue 的最大 global work size（`0` 表示使用内部计算的默认值）。
- `--inverse-size <N>`（默认 `255`）、`--inverse-multiple <N>`（默认 `16384`）：影响 GPU 端批处理规模/预处理表大小，从而影响速度与显存占用。

如果你只是想“更快”，优先考虑：更新显卡驱动、减少 CPU/IO 干扰、合理选择 `--skip`，而不是盲目改这些参数。

---

## 3. 目录结构说明

### 3.1 发布目录（运行所需）

`dist\`

- `profanity.exe`：主程序
- `OpenCL.dll`：运行时 OpenCL DLL
- `libcrypto-3-x64.dll`：OpenSSL 运行时依赖
- `result\`：输出结果目录（脚本默认写这里）
- `cache\`：OpenCL kernel 二进制缓存目录（可删，会自动重建）
- `matching\`：匹配规则文件目录（例如 `digits.txt` / `letters.txt`）
- `scripts\`：辅助启动脚本

### 3.2 源码/工程目录（开发所需）

- `profanity.sln` / `profanity.vcxproj`：Visual Studio/MSVC 工程文件
- `profanity.cpp`：程序入口与参数解析
- `Dispatcher.*`：OpenCL 设备枚举、任务调度、结果处理与输出
- `Mode.*`：匹配输入解析（支持“文件/单地址”两种来源）
- `kernel_*.hpp`：OpenCL kernel 相关代码（内嵌为字符串/头文件形式）
- `OpenCL\` / `Openssl\`：编译所需的头文件与导入库

---

## 4. 从源码编译（开发者）

### 4.1 环境要求

1. Windows 10/11 x64
2. Visual Studio 2022 或 Visual Studio 2022 Build Tools，并安装：
   - Desktop development with C++
   - MSVC v143 toolset
   - Windows 10/11 SDK
3. 支持 OpenCL 的 GPU 驱动（NVIDIA / AMD / Intel）

仓库已包含工程所需的 `OpenCL\` 与 `Openssl\` 头文件/导入库。

### 4.2 构建 Release x64

方式 A：用 Visual Studio 打开 `profanity.sln`，选择 `Release|x64` 构建。

方式 B：命令行（PowerShell）使用已安装的 MSBuild：

```powershell
MSBuild.exe ".\profanity.sln" /t:Rebuild /p:Configuration=Release /p:Platform=x64 /m
```

构建产物默认在：

```text
build\bin\x64\Release\profanity.exe
```

如果要发布给普通用户，将其复制覆盖到：

```text
dist\profanity.exe
```

---

## 5. 二次开发（二开）说明

### 5.1 代码入口与关键流程

1) `profanity.cpp`

- 解析命令行参数（包含一些 `--help` 未列出的高级参数：`--work` / `--work-max` / `--inverse-size` / `--inverse-multiple` / `--no-cache` 等）。
- 构造 `Mode::matching(...)`（把 matching 输入转为内部掩码/数据数组）。
- 枚举 OpenCL 设备并创建 `Dispatcher`，将设备加入调度。

2) `Mode.cpp`

- `--matching` 支持两类输入：单个地址（34 位）或匹配文件路径。
- 匹配文件逐行读取：只接受长度为 `20` 或 `34` 的行；长度为 `34` 时会先移除中间 14 位，最终都变成“前10+后10”的 20 字符形式。

3) `Dispatcher.*` / `kernel_*.hpp`

- `Dispatcher` 负责 OpenCL program 构建、kernel 调度、异步回调、结果筛选与输出。
- kernel 相关逻辑在 `kernel_profanity.hpp` / `kernel_sha256.hpp` / `kernel_keccak.hpp` 等头文件中。

### 5.2 常见二开方向

- **补全/对齐帮助信息**：如果你增加了新的参数或希望把高级参数展示在 `--help` 中，可同步更新 `help.hpp`（`g_strHelp`）。
- **扩展匹配逻辑**：
  - 只改“输入规则/格式”：优先改 `Mode::matching(...)`。
  - 改“评分/匹配算法”：需要同时调整 `Mode` 的数据组织方式与 kernel 的匹配/打分逻辑。
- **性能调优**：通常围绕 `--work`、`--work-max`、`--inverse-*` 以及 kernel 实现；建议在固定驱动版本与设备上对比测试，避免“改快了 A 卡但拖慢 B 卡”。
- **输出格式/落盘策略**：在 `Dispatcher.cpp` 的 `writeResult(...)` 与 `printResultX(...)` 附近修改。

### 5.3 打包与分发注意事项

分发给普通用户时，请打包整个 `dist\` 目录，而不是只复制 `profanity.exe`：

- `profanity.exe`
- `OpenCL.dll`
- `libcrypto-3-x64.dll`
- `matching\`（如果你依赖仓库提供的规则/脚本）
- `scripts\`（可选）

### 5.4 安全提醒（开发/使用都适用）

- 默认输出包含明文私钥，请务必离线保存、妥善保管。
- 在真实资产使用前，先独立验证“私钥 ↔ 地址”是否匹配。
- 不建议把生成的钱包直接作为高价值主钱包；建议使用多签或额外安全措施。
