# ToyboxSpikes (SpikesKit)

## 📌 Overview

`ToyboxSpikes` は、次世代自作ゲームエンジン（LREngine）へ組み込むためのコア・アーキテクチャを先行検証（Spike）するためにフルスクラッチで開発された、C++20 / DirectX 11 ベースの技術検証用サンドボックスです。

RHI (Rendering Hardware Interface) の抽象化検証にとどまらず、以下のようなモダンエンジンの根幹となる様々な低レイヤ技術を迅速かつ反復的に検証・実装しています。

* **マルチスレッド基盤:** `std::atomic` を駆使した完全ロックフリーな `RenderJobSystem` の構築。

* **パフォーマンスと保守性の両立 (Hybrid Architecture):** `RenderPass` 等の構造管理層にはOOPを採用して拡張性を担保しつつ、高負荷な描画コマンド構築層にはデータ指向設計（DOD）を適用。仮想関数を排除したフラットなコマンド構造と `FrameAllocator` の組み合わせにより、毎フレームの動的メモリ確保を完全に排除。

* **アセット・通信インフラ:** 独立したインポーターによる非同期ロードパイプラインの実装と、軽量・型安全なイベント駆動アーキテクチャ（`EventDispatcher`）の導入。

将来的な DirectX 12 / Vulkan への移行を前提とし、保守性と実行速度のトレードオフをシビアに見極めた「堅牢な（Robust）インフラ設計」の証明として機能します。

## 🚀 Demonstrations

本リポジトリには、検証用途に応じた以下のデモ・アプリケーションが含まれています。

* **OpaqueRenderingDemo**

  * Sponza 2022 Scene を用いた不透明描画テスト。

  * Tracy Profiler を組み込み、`RenderJobSystem` による並列コマンド構築スループットのパフォーマンスを計測・実証します。

* **TransparentRenderingDemo**

  * アルファテスト (Mask) および Zライト制御 (Depth Write Zero) を伴う、半透明 (Alpha Blending) 描画パスの検証デモ。

  * *(※厳密な半透明Zソート機構の構築は、既知のIssueとして今後の拡張ロードマップへ定義)*

### 🎮 動作確認用コントロール (Controls)

ビルド実行後、シーン内を自由に移動してご確認いただけます。

* **カメラ移動:** W A S D

* **視点操作:** 右クリック を押しながらマウス移動 (Free Camera)

## 🏛️ Core Architecture & Design Philosophy

### 1. RenderJobSystem: Lock-Free Command Building

※汎用的なスレッドプールではなく、「描画コマンドの並列構築」に特化・隔離させたロックフリー基盤です。重い `std::mutex` などの OS ロックを描画ループから完全に排除し、CPU のハードウェア命令（`std::atomic`）を直接制御することで、コマンド構築フェーズの限界スループットを実現しています。

* **SPMC Ring Buffer:** `compare_exchange_weak` (CAS操作) とビット論理積 (`& 511`) を駆使した、ロックフリーなジョブキュー。

* **CPU-CPU Synchronization:** C++20 `std::latch` によるアトミック・ダウンカウンター方式を採用。メインスレッド側の同期待機オーバーヘッドを削減。

### 2. Data-Oriented Design (DOD) & Memory Management

動的アロケーション（ヒープ確保）によるキャッシュ破壊とスレッド渋滞を防ぐため、徹底したデータ駆動設計を適用しています。

* **Dual Bump Allocator:** 毎フレームリセットされる 32MB の連続メモリ（Frame Allocator）から、各ワーカースレッドが 64KB のブロックをロックフリーで切り出すリニア・アロケータ設計。

* **Zero-Cost Abstraction:** ファントム型（`TypedHandle<T>`）と `SlotMap` を組み合わせることで、実行時の VTable オーバーヘッドを完全に排除しつつ、O(1) の高速なアセットアクセスと型安全性を両立。

### 3. RHI Abstraction

API に依存しないフロントエンド（コマンド構築層）と、DX11 API を直接制御するバックエンドを完全に分離しています。

* **PSO Emulation:** DX11 上でありながら、DX12 の Root Signature や PipelineStateObject (PSO) に準拠した設計をエミュレーション。

* **Safe ABI Boundary:** DLL 境界を越える際、異なるCランタイム（CRT）間のヒープ破損（Heap Corruption）リスクを回避するため、内部 `Release()` (`delete this;`) を用いた確実な寿命管理を実施。

### 4. Asynchronous Asset Pipeline

* **Stateless glTF 2.0 Importer:** インポーターから状態変数を排除したステートレス設計（純粋関数化）により、複数スレッドからの同時アクセス（デコード）を安全に実行。

* **Minimized Critical Sections:** ファイル I/O とコンパイルをスレッドローカルで実行し、最後の SlotMap 登録時のみ Read-Write Lock を使用することで、メインスレッドのブロッキングを最小化。

## 🛠️ Build Instructions

本プロジェクトは Windows 環境および Visual Studio 向けに最適化された CMake ビルドシステムを構築しています。開発者の環境構築負荷（I/O）を最小化するため、自動セットアップ用のバッチスクリプトを用意しています。

### 1. Requirements

* **OS:** Windows 10 / 11

* **IDE / Compiler:** Visual Studio 2022 または 2026 (MSVC, C++20 サポート必須)

* **Build Tool:** CMake 3.xx+

* **SDK:** Vulkan SDK (Vulkan API バックエンド検証用)

### 2. Setup & Build

複雑な CMake コマンドの手動入力は不要です。

1. リポジトリ・ルートにある `SetupVS2022.bat` または `SetupVS2026.bat` を環境に合わせて実行します。

2. 自動的に `build-msvc` ディレクトリと Visual Studio のソリューションファイル（`.sln`）が生成されます。

3. 生成されたソリューションを起動し、Visual Studio 上からビルドを実行してください。

### 3. Assets Setup

リポジトリのクローン速度最適化（容量削減）のため、巨大なメッシュやテクスチャ等のアセットは Git 管理から除外しています。デモを正常に実行するために、初回のみ以下の手順が必要です。

1. 本リポジトリの **Releases** ページから、アセットパックのアーカイブをダウンロードします。

2. アーカイブを展開し、中身のデータをプロジェクト内の `Asset/Mesh/` ディレクトリ配下に配置してください。

3. デモアプリケーションを実行し、正常にレンダリングされることを確認します。

## ⚖️ Credits & Licenses

本プロジェクトのデモ実行用として、以下のサードパーティ製 3D モデルデータ（アセット）を使用させていただいております。
アセットの著作権は、各クリエイターおよび権利者に帰属します。

* **Alpha Blend Mode Test**

  * **License:** [CC BY 4.0 International](https://creativecommons.org/licenses/by/4.0/legalcode) (SPDX: CC-BY-4.0)

  * **Source:** [glTF-Sample-Models](https://github.com/KhronosGroup/glTF-Sample-Models.git)

* **Sponza Base Scene**

  * **Authors:** Frank Meinl and Anton Kaplanyan (Commissioned by Frank Meinl, sponsored by Anton Kaplanyan)

  * **Sponza Addon Package Crew:** Katica Putica, Cristiano Siqueira, Timothy Heath, Justin Prazen, Sebastian Herholz, Bruce Cherniak, Anton Kaplanyan

  * **Additional credit for reference photos:** Katica Putica, Princino.photo (www.princinophoto.com), Dubrovnik, Croatia

  * **Source:** [Intel Sample Library](https://www.intel.com/content/www/us/en/developer/topic-technology/graphics-processing-research/samples.html)

  * **License:** [Creative Commons Attribution 4.0 International (CC BY 4.0)](https://creativecommons.org/licenses/by/4.0/legalcode)

*(※上記アセットデータは本リポジトリのソースコードには含まれず、開発者のローカル環境および外部ストレージでのみ検証用途として利用しています。)*

本プロジェクトの C++ ソースコードは MIT License の下で公開されています。