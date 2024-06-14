#set text(font: ("linux libertine", "Source Han Serif SC", "Source Han Serif"))

#align(right + horizon)[
  #image("imgs/front.png")

  = MMD / Unknown Mother Goose / 卡通渲染测试

  2024 春 计算机图形学大作业报告

  *小组成员*

  08 马天开

  06 陈冠宇
]

#pagebreak()

#set page(footer: context [
  #let selector = selector(heading.where(level: 2)).before(here())
  #let level = counter(selector)
  #let headings = query(selector)
  #let heading = headings.last()

  *MMD / Unknown Mother Goose / 卡通渲染测试* / #text(size: 8pt, heading.body)
  #h(1fr)
  #counter(page).display(
    "1/1",
    both: true,
  )
])

== 简介

本次大作业以歌曲「Unknown Mother Goose」作为背景音乐, 利用 Blender、MMD、Davinci 等工具完成了一个 MMD 视频 (一类 3D 动画) 的制作. 我们讨论并分析「卡通渲染」的一些方法, 并在制作的过程中对使用的工具进行一定程度的改进.

== 背景

本次作业中, 人物模型、动作以及镜头都来自 MMD 社区, 以 `.pmx` (模型文件) ` .vmd` (动作文件) 等格式分发, 首要任务就是如何将这些文件导入到 Blender 中进行渲染.

=== MMD

MMD <#link("https://en.wikipedia.org/wiki/MikuMikuDance")> 是一款由 Yu Higuchi 制作的免费 3D 动画制作软件, 尽管在 2011 年, MMD 的作者宣布停止软件的维护, 但 MMD 以其简单易用、社区活跃等特点仍然受到广泛欢迎.

#align(center)[
  #image("imgs/mikumikudance.png", width: 60%)
]

在 2024 年, 我们不得不注意到 MMD 软件存在的一些问题:

- 与其他软件兼容性问题:

  MMD 为了方便用户使用, 集成了模型、动作、渲染等功能于一体, 但是对于想要做出更加细致调节的用户来说, 唯一的视频导出的选项显得不是那么友好.

- 平台兼容性问题:

  MMD 依赖 DirectX 9 API, 这使得其只能运行在 Windows 平台上, 并且随着 Windows 更新, DirectX 早期版本的支持也将成为问题.

针对以上问题, MMD 社区也对应的解决 (替代) 方案:

- 活跃于社区的 MMD 替代品: MMM <#link("https://sites.google.com/site/mikumikumovingeng")>, 最新版本发布于 2018 年. (同样只支持 Windows 平台)

  #align(center)[
    #image("imgs/mikumikumoving.png", width: 60%)
  ]

- MMD 导出 Alembic 文件的插件: MMDBridege <#link("https://github.com/uimac/mmdbridge")>, 是对 MMD 本体的插件.

  #align(center)[
    #image("imgs/mmdbridge.png", width: 60%)
  ]

- Nanoem <#link("https://github.com/hkrn/nanoem")> Nanoem 是一款开源 (MIT License) 的 MMD 替代品, 目标平台是 Windows, macOS 和 Linux, MMD 本体的功能已经基本被实现.

  #align(center)[
    #image("imgs/nanoem.png", width: 60%)
  ]

- Blender mmd_tools 插件 <#link("https://github.com/UuuNyaa/blender_mmd_tools")>, 不再依赖 MMD 本体, 而是直接使用 Blender 导入对应模型和动作.

  #align(center)[
    #image("imgs/mmd_tools.png", width: 30%)
  ]

  // 值得一提的是, Blender 在进行物理解算时, 并*不会*特殊处理人物模型, 比如在处理人物模型时我们经常会「砍手」来减少穿模问题发生的频次, 这些都需要额外的处理. Blender 并不会自动处理这些问题. 也就是说, Blender 内置的通用物理解算并不适用于 MMD 模型.

=== MMD 文件格式

在 MMD 中主要使用到 `.pmx` (模型文件) 和 `.vmd` (动作文件) 两种文件格式:

- `.pmx` 文件:

  格式标准位于: <#link("https://gist.github.com/felixjones/f8a06bd48f9da9a4539f")>, 作为 PMD 格式的后继, PMX 格式在 2010 年发布, 与 PMD 格式相比, PMX 格式支持更多的功能, 例如骨骼数量、材质数量等.

  #align(center)[
    #image("imgs/pmx.png", width: 50%)
  ]

  在此基础上, 不同模型之间有另一套约定俗成的标准, 例如: 骨骼的命名, 材质的命名等, 比较知名的有 Sour式, 三妈式等. 由于需要保持模型跟动作之间对骨骼的对应关系, 一般来说模型和动作的作者都会在分发时表明使用的标准.

- `.vmd` 文件:

  格式标准位于: <#link("https://mikumikudance.fandom.com/wiki/VMD_file_format")>, VMD 文件是 MMD 中的动作文件, 用于描述骨骼的运动.

  #align(center)[
    #image("imgs/vmd.png", width: 50%)
  ]

=== 角色动画

我们在这里补充一些关于本次作业使用到关于角色动画的一些基础概念:

- IK 约束: Inverse Kinematics, 逆向运动学.

  #align(center)[
    #image("imgs/ik.png", width: 50%)
  ]

  IK 的作用在于, 只需要指定最后一跟骨骼的位置, 其他骨骼的位置会自动计算出来, 这是对动画过程一种极大的简化.

- SDEF: Spherical Deform, 球形变形.

  SDEF 一般作用在关节处, 如手肘、膝盖等, 用于模拟关节的弯曲效果. 只有正确设置了 SDEF, 才能使得角色动画看起来更加自然.

  #align(center)[
    #image("imgs/sdef.png", width: 50%)
  ]

=== Alembic

Alembic <#link("https://en.wikipedia.org/wiki/Alembic_(computer_graphics)")> 是一种用于交换复杂动画数据的开放格式, 它的特点是可以存储大量的顶点、法线、纹理等数据. 在 2024 年, Alembic 格式已经被广泛使用, 例如在电影制作中, 动画片制作中, 甚至是游戏开发中都可以看到 Alembic 格式的身影.

在本次作业中, `.abc` 文件用作物理解算结果 $<=>$ Blender 之间的数据交换格式.

#pagebreak(weak: true)

== 任务

=== Stage 1. 物理解算

我们小组讨论了一些导入 `.pmx` 文件, 进行物理解算的方式, 以下是两个有可能的技术路线:

+ 适当改进 mmd_tools 插件对 Blender 4.1 的支持, 具体表现为在 Blender 4.1 上烘焙物理时容易发生内存溢出的问题, 读写文件、修改动作时容易出现错误

+ 考虑到 Nanoem 项目代码开源, 可以在此基础上添加导出 Alembic 文件的功能, 并且适当改进其物理解算的算法

=== Stage 2. #highlight[卡通渲染]

接下来是本次作业的一个亮点, 如何在 Blender 中实现卡通渲染的效果, 我们讨论了以下几个值得回答的问题:

- 卡通勾线

  - 不同方式的勾线有什么优缺点? 性能如何? 效果如何?

  - 如何在 Blender 中实现这些效果?

- 卡通着色

  - 卡通着色的原理是什么? 如何在 Blender 中实现?

- 动画效果

- 提高 Blender 渲染速度

=== Stage 3. 后期制作

一个优秀的 MMD 视频不仅仅是模型、动作的堆砌, 后期制作需要完成字幕, 特效, 调色等工作.

#pagebreak(weak: true)

== Stage 1. 物理解算

=== Blender & mmd_tools

在深入调研 mmd_tools 的开发情况后, 我们意识到这些问题主要来自 Blender 4.1 上游而不是插件本身. 尽管无法对 mmd_tools 做出改良, 我们依旧尝试了使用其导入模型:

#image("imgs/mmd_tools_import-0.png")

mmd_tools 同时会处理好 shader 的默认表现, 这与 MMD 中的效果保持一致.

#image("imgs/mmd_tools_import.png")

但是在导入动作之后问题就显现出来:

#image("imgs/mmd_tools_import-1.png")

这里导入的 padding 推荐设置为 20, 原因是: mmd_tools 会保留 A-pose 作为第一帧, 而动作从 padding 开始计算. 如果不增大 padding, 会导致稍后的物理解算出现严重问题, 包括但不限于裙子穿模、动作乱飞等.

导入后直接点击播放的话, 此时并没有进行物理解算:

#align(center)[
  #image("imgs/mmd_tools_import-2.png", width: 70%)
]
#box(width: 100%)[
  此时需要点击模型的根节点, 在插件面板上依次选择开启物理效果, 烘焙物理:

  #align(center)[
    #image("imgs/mmd_tools_import-4.png", width: 50%)
    #image("imgs/mmd_tools_import-3.png", width: 50%)
  ]
]
但是这样的解算依然是存在问题的, 举例来说:

#align(center)[
  #image("imgs/blender-physics-error.png", width: 50%)
]

在 Blender 中修复这类问题的成本过于巨大, 我们对于 Blender 物理解算的探索到此为止.

#pagebreak(weak: true)

=== Nanoem 改进

尝试过 Blender + mmd_tools 的组合后, 我们也意识到到标准 abc 流程对于效率的提升, 可以将前期物理、布料解算的结果预先处理, 再导入到 Blender 进行下一步操作.
// 同时也能对 MMD 模型做特殊的处理, 这一点我们在下文中会提到.

// 综合以上原因,
我们尝试对 Nanoem 做一点改进:

+ Alembic 支持的部分主要参考了以下来源:

  - Alembic 代码库 <#link("https://github.com/alembic/alembic")>

  - MMD Bridge 导出部分

  与 MMD Bridge 类似, 我们在 Nanoem 导出视频时注入了 Alembic 导出的代码:

  ```c
  motion->accessory_keyframes = (nanoem_motion_accessory_keyframe_t **) nanoem_calloc(num_accessory_keyframe, sizeof(*motion->accessory_keyframes), status);
  if (nanoem_is_not_null(motion->accessory_keyframes)) {
    for (i = 0; i < num_accessory_keyframe; i++) {
        accessory_keyframe_message = accessory_keyframe_bundle_message->keyframes[i];
        // injection here
  ```

  经过上面的处理后, 我们能正确导入到 Blender, 但是仍然存在一些解算的问题:

  #align(center)[
    #image("imgs/bad_export.jpg", width: 40%)
  ]

+ 修改 Nanoem 关于物理解算的部分, 添加头发与裙子的特殊处理, 可以得到正确的解算结果:

  #align(center)[
    #image("imgs/good_export.jpg", width: 70%)
  ]

+ #link("https://www.bilibili.com/video/BV1PM4m1r7Nk?p=1")[这里] 存放着一个不含物理解算的结果(视频); #link("https://www.bilibili.com/video/BV1PM4m1r7Nk?p=2")[这里] 存放着正确导出物理解算的结果(视频).
// + 对 Nanoem 修改部分的代码已随报告打包, 代码不是很稳定, 推荐直接使用导出的 `.abc` 文件.
// , 这些代码目前只适配 macOS 平台 (用到 Metal API), 今年晚些时候会整理成 PR 提交给上游.

#pagebreak(weak: true)

== Stage 2. 卡通渲染

=== Blender 导入

接下来是使用 Blender 进行渲染的一些过程. 经过多次试错, 我们推荐按照如下方式将上文的 `.abc` 文件导入到 Blender:

+ 使用已经停止支持的 Blender 2.9 版本导入 `.abc`, 依次选择 `File -> Import -> Alembic (.abc)`, 选择导入的文件即可, 由于 `.pmx` 标准问题, 导入时应该选择 `0.08` 的缩放比例.

  #align(center)[
    #image("imgs/blender-import-abc-menu.png", width: 40%)
    #image("imgs/blender-import-abc-window.png", width: 60%)
  ]

+ 按空格键播放动画, 确认导入的动画效果正确.

  #align(center)[
    #image("imgs/blender-import-abc-test.png", width: 60%)
  ]

+ 将导入的 mesh 合并到一个 Collection 下, 方便复用.

  #align(center)[
    #image("imgs/blender-import-abc-collection.png", width: 30%)
  ]

+ 导出的 `.abc` 文件不包含颜色信息, 但是我们的导出工具已经将材质贴图一并输出到 `out/` 目录下, 打开这个目录, 分别创建三个 shader, 对应三张材质贴图. UV 信息应该正确包含在 `.abc` 导出的结果中, 只需要将这些 shader 对应到 mesh 上即可.

  #align(center)[
    #image("imgs/blender-import-abc-shader.png")
  ]

#pagebreak(weak: true)

=== 主场景

经过上面的操作之后, 我们得到了一个 `.blend` 文件, 包含物理解算之后得到的顶点运动数据等, 我们接下来使用 Blender 4.1 进行操作:

+ 追加人物模型到当前场景, 依次选择 `File -> Append -> Object -> Collection`, 选择导入的 Collection 即可.

  #align(center)[
    #image("imgs/blender-2nd-import.png", width: 30%)
  ]

+ 按空格键播放动画, 确认导入的动画效果正确.

+ 在此基础上添加场景, 确认相对位置正确.

  #align(center)[
    #image("imgs/blender-main-scene.png")
  ]

#pagebreak(weak: true)

=== 卡通渲染管线

在实际制作这些管线之前, 我们应该了解卡通渲染的几个方向, 分别是: #highlight[卡通勾线], #highlight[卡通着色], 动画效果.

+ 卡通勾线

  业界目前的勾线大致分为四类, 几何勾线、资源勾线、屏幕空间勾线、Ray Tracing 勾线, 我们来观察其中的几种:

  + 几何勾线:

    #align(center)[
      #image("imgs/blender-gs-stroke-demo.png", width: 52%)
    ]

    大致思路: 使用法线将模型挤出一定距离, 用于勾线的渲染. 这种方法性能很好, 可以控制粗细 (进而可以设置勾线粗细的远近效果), 但是无法处理模型内部的勾线.

    在 Blender 中, 我们可以使用一个几何着色器来实现这一效果:

    #image("imgs/blender-gs-stroke.png")

    在挤出之后, 需要对模型做一些细微的设置:

    #image("imgs/blender-gs-stroke-main.png")

  + 资源勾线:

    大致思路: 在贴图中直接包含勾线线条, 但是这种方法对贴图精度高, 低精度贴图会导致锯齿效果, 而且无法无破坏的修复勾线, 毕竟这是贴图的一部分.

    事实上, 这次我们选用的模型, 由于实际上是游戏资源的拆包, 可以看出部分模型的勾线是直接包含在贴图中的.

    #align(center)[
      #image("imgs/blender-asset-stroke.png", width: 40%)
    ]

    对游戏开发而言, 额外的资产意味着额外的工作量, 但是为应对手机等环境, 这种方法仍然是一种不错的选择.

  + 菲涅耳勾线:

    大致思路: 在模型表面根据法线角度计算勾线, 这种方法可以很好的处理模型内部的勾线, 但是对于模型表面的勾线控制较差.

    #align(center)[
      #image("imgs/blender-fresnel-stroke-demo.png", width: 40%)
    ]

    在 Blender 中, 我们可以使用一个菲涅耳着色器来实现这一效果:

    #image("imgs/blender-fresnel-stroke.png")

    此方法的缺点是, 相机接近模型时, 由于精度问题, 会出现不同程度的渲染错误:

    #align(center)[
      #image("imgs/blender-fresnel-stroke-failure-demo.png", width: 80%)
    ]

  + 屏幕空间勾线:

    大致思路: 在屏幕空间中计算勾线, 一般可以用 深度 + 法线等数据进行边缘查找.

    Blender 中的 FreeStyle 是一种基于屏幕空间的勾线渲染方式:

    #align(center)[
      #image("imgs/blender-freestyle-enable.png", width: 20%)
    ]

+ 卡通着色

  卡通材质是一类抽象程度较高的表现方式, 从表面属性来说, 一般有着以下几种维度:

  - 高光
  - 亮部颜色
  - 阴影过渡
  - 阴影颜色

  首先我们来实现二分光照模型:

  #align(center)[
    #image("imgs/blender-toon-shading.png")
  ]

  接下来与贴图结合:

  #align(center)[
    #image("imgs/blender-toon-shading-processing.png")
  ]

  计算高光:

  #align(center)[
    #image("imgs/blender-highlight.png", width: 80%)
  ]

  #align(center)[

    #image("imgs/blender-highlight-2.png")
  ]

  边缘光的计算:
  #align(center)[
    #image("imgs/blender-edge-light.png")
  ]

  #align(center)[
    #image("imgs/blender-edge-light-2.png")
  ]

+ 动画效果

  部分三渲二效果为追求和手绘动画的一致性, 会进行抽帧等处理, 我们可以通过分别渲染场景和人物, 人物动画一拍二, 背景一拍一的方式来实现类似的效果.

  但是这种处理对于 MMD 动画来说, 会增加动作的不连贯感, 并且不适合动作幅度较大的庆幸, 所以我们不推荐这种处理方式.

#v(2em)

结合上面的内容, 我们可以得到 Blender 渲染的效果:

#image("imgs/blender-output.png")

#pagebreak(weak: true)

== Stage 3. 后期制作

在完成了上述的步骤之后, 我们接下来需要完成一些后期制作的工作:

#image("imgs/davinci.png")

为了实现上面的遮罩效果, 我们需要额外在 Blender 中渲染一个只包含人物的 mask, 类似下图:

#image("imgs/mask.png")

在 Blender 中实现上述输出的一个技巧是, 关闭场景建模, 将雾场通道渲染到输出:

#align(center)[
  #image("imgs/fog.png", width: 30%)
]

在合成节点中输出:

#align(center)[
  #image("imgs/fog-node.png", width: 60%)
]

至此, 我们完成了一个 MMD 视频的制作.

== 分工

- 马天开: 负责 Blender 渲染部分, 包括卡通勾线、卡通着色等效果的实现, 以及后期制作的部分.

- 陈冠宇: 负责前期调研, Nanoem 的改进, 包括 Alembic 文件的导出, 物理解算等部分.

== 感想

+ 通过本次作业, 小组成员首次接触了 MMD 制作的流程, 深入学习使用了 Blender 软件, 了解了卡通渲染的一些方法, 也对 Alembic 格式有了更深入的了解.

+ 小组成员的团队协作能力得到了锻炼, 尤其在分工合作、任务分配等方面.

== 不足

+ 由于时间关系, 我们没有对 Nanoem 的代码进行更深入的改进

+ Blender 素材整理较为混乱, 技巧性不足

+ 后期效果可以更加完善

== 致谢

特别感谢刘老师和助教们的指导和付出, 感谢 MMD 社区中分享模型、动作的作者们.