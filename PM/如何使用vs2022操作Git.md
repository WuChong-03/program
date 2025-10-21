## Visual Studio 的 Git图形化功能

- 在过去，想要图形化地操作 Git，一般需要借助外部 GUI 工具，比如 SourceTree。
- 但从 Visual Studio 2019（版本 16.6） 开始，
微软在 IDE 里正式加入了一个名为 Git Experience 的新功能，
让开发者可以直接在 Visual Studio 内使用 Git。

## 操作流程

### 1. 连接 Git 账户。

- 在 Visual Studio 登录时，可以选择使用自己的 GitHub 账户登录。
（默认登录选项是 微软账户（Microsoft Account），

- 若使用微软账户登录，则仍需另外绑定 GitHub 账户才能使用 Git 功能。

### 2. 仓库操作 /建立/克隆/读取

#### 🧩建立仓库

建立仓库有两种方式:

- 方法①：用 Visual Studio 直接推送创建（自动方式）

    - 若当前项目没有被 Git 管理（即文件夹里没有 .git 文件夹），
    只要在菜单中点击 「推送到 Git 服务 (Push to Git Service)」，

    - 📘 适用场景：
    学校项目、小型作业、个人练习项目。

- 方法②：自己手动构建仓库结构（手动方式）

    - 如果你希望自己决定文件夹层级或仓库内容，
    可以选择手动初始化仓库并连接 GitHub。但需要一定命令行基础（Git Bash / 终端）
        ```
        git init                     # 初始化仓库
        git add .                    # 添加所有文件
        git commit -m "Initial commit"  # 创建第一次提交
        git remote add origin https://github.com/用户名/仓库名.git  # 绑定远程仓库
        git push -u origin main      # 推送到 GitHub
        ```

    - 📘 适用场景：
    大型项目、多项目共仓、手动规划目录结构的开发。

---

#### 🧩克隆

“克隆” = 从远程仓库下载一份副本。

- 若你的 GitHub 账户中已有仓库，
  或他人与你共享了一个项目仓库，
  可以使用“克隆”功能将其下载到本地。

- 菜单 → Git → 克隆仓库 (Clone Repository)

---

#### 🧩读取

如果你本地已经有一个 Git 仓库（即文件夹内有 .git），
可以直接在 Visual Studio 中打开它。

- 菜单 → Git → 本地存储库 → 打开本地仓库

---

### 3. Git 基本操作：Commit、Push、Pull

#### 🧱commit 
把你当前的修改保存到本地仓库（不会上传到 GitHub）。
- Git → 提交或存储
#### 🚀push
把你在本地提交的更改上传到远程仓库（GitHub）。
- Git → 提交或存储 → 全部提交
#### 🔄pull
从 GitHub 获取最新版本代码，并更新到你的本地仓库。
- 菜单 → Git → 拉取

### 4. 分支Branch / 主分支Master / 合并merge
在 Git 中，分支（branch） 就像是项目的“平行世界”。
可以在不破坏主项目的情况下，
创建一个独立空间来编写、测试新功能。
当你确认新功能没有问题后，再把它合并（merge）回主分支。
    
