# ToyboxSpikes (SpikesKit)

## 📌 Overview
`ToyboxSpikes` は、次世代自作ゲームエンジン（LREngine）へ組み込むためのコア・アーキテクチャを先行検証（Spike）するためにフルスクラッチで開発された、C++20 / DirectX 11 ベースの技術検証用サンドボックスです。

RHI (Rendering Hardware Interface) の抽象化検証にとどまらず、以下のようなモダンエンジンの根幹となる様々な低レイヤ技術を爆速で検証・実装しています

*   **マルチスレッド基盤:** `std::atomic` を駆使した完全ロックフリーな `RenderJobSystem` の構築。
*   **メモリ・アセット管理:** データ指向設計（DOD）に基づくリソースのキャッシュ最適化と、非同期ロードパイプラインの実装。
*   **通信インフラ:** 軽量かつ型安全なイベント駆動アーキテクチャ（`EventDispatcher`）の導入。

将来的な DirectX 12 / Vulkan への移行を前提とし、保守性と実行速度のトレードオフをシビアに見極めた「崩壊しないインフラ設計」の証明として機能します。

## 🚀 Demonstrations
本リポジトリには、検証用途に応じた以下のデモ・アプリケーションが含まれています。
*   **OpaqueRenderingDemo**
    *   Sponza 2022 Scene を用いた 不透明描画テスト。
    *   Tracy Profiler を組み込み、`RenderJobSystem` による並列コマンド構築スループットの限界を検証します。
*   **TransparentRenderingDemo**
    *   アルファテスト (Mask) / Zライト制御 (Depth Write Zero) を伴う半透明 (Alpha Blending) 描画の実証デモ。
    (※半透明の厳密なZソート機構は今後の拡張ロードマップとして定義)

## 🏛️ Core Architecture & Design Philosophy

### 1. RenderJobSystem: Lock-Free Command Building
※汎用的なスレッドプールではなく、「描画コマンドの並列構築」に特化・隔離させたロックフリー基盤です。重い std::mutex などの OS ロックを描画ループから完全に排除し、CPU のハードウェア命令（std::atomic）のみを直接叩くことで、コマンド構築フェーズの限界スループットを実現しています。

SPMC Ring Buffer: compare_exchange_weak (CAS操作) とビット論理積 (& 511) を駆使した、ロックフリーなジョブキュー。

CPU-CPU Synchronization: C++20 std::latch によるアトミック・ダウンカウンター方式を採用。メインスレッド側の同期待機オーバーヘッドを削減。

### 2. Data-Oriented Design (DOD) & Memory Management
動的アロケーション（ヒープ確保）によるキャッシュ破壊とスレッド渋滞を防ぐため、徹底したデータ駆動設計を適用しています。
*   **Dual Bump Allocator:** 毎フレームリセットされる 32MB の連続メモリ（Frame Allocator）から、各ワーカースレッドが 64KB のブロックをロックフリーで切り出す土管設計。
*   **Zero-Cost Abstraction:** ファントム型（`TypedHandle<T>`）と `SlotMap` を組み合わせることで、実行時の VTable オーバーヘッドを完全に排除しつつ、O(1) の高速なアセットアクセスと型安全性を両立。

### 3. RHI Abstraction
API に依存しないフロントエンド（コマンド構築層）と、DX11 API を叩くバックエンドを完全に分離しています。
*   **PSO Emulation:** DX11 上でありながら、DX12 の Root Signature や PipelineStateObject (PSO) に準拠した設計をエミュレーション。
*   **Safe ABI Boundary:** DLL 境界を越える際、Cランタイムの違いによるヒープの「島」問題（Heap Corruption）を回避するため、内部 `Release()` (`delete this;`) を用いた確実な寿命管理を実施。

### 4. Asynchronous Asset Pipeline
*   **Stateless glTF 2.0 Importer:** インポーターから状態を剥奪した純粋関数化により、複数スレッドからの同時アクセス（デコード）を安全に実行。
*   **Minimized Critical Sections:** ファイル I/O とコンパイルをスレッドローカルで実行し、最後の SlotMap 登録時のみ Read-Write Lock を使用することで、メインスレッドのブロッキングを完全に排除。

## 🛠️ Build Instructions
本プロジェクトは Windows 環境および Visual Studio 向けに最適化された CMake ビルドシステムを構築しています。開発者の環境構築負荷（I/O）を最小化するため、自動セットアップ用のバッチスクリプトを用意しています。

### 1. Requirements
*   **OS:** Windows 10 / 11
*   **IDE / Compiler:** Visual Studio 2022 または 2026 (MSVC, C++20 サポート必須)
*   **Build Tool:** CMake 3.xx+
*   **SDK:** Vulkan SDK (Vulkan API バックエンド検証用)

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

*   **Alpha Blend Mode Test**
    *   **License:** [CC BY 4.0 International](https://creativecommons.org/licenses/by/4.0/legalcode) (SPDX: CC-BY-4.0)
    *   **Source:** [glTF-Sample-Models](https://github.com/KhronosGroup/glTF-Sample-Models.git)

*   **Sponza Base Scene**
    *   **Authors:** Frank Meinl and Anton Kaplanyan (Commissioned by Frank Meinl, sponsored by Anton Kaplanyan)
    *   **Sponza Addon Package Crew:** Katica Putica, Cristiano Siqueira, Timothy Heath, Justin Prazen, Sebastian Herholz, Bruce Cherniak, Anton Kaplanyan
    *   **Additional credit for reference photos:** Katica Putica, Princino.photo (www.princinophoto.com), Dubrovnik, Croatia
    *   **Source:** [Intel Sample Library](https://www.intel.com/content/www/us/en/developer/topic-technology/graphics-processing-research/samples.html)
    *   **License:** [Creative Commons Attribution 4.0 International (CC BY 4.0)](https://creativecommons.org/licenses/by/4.0/legalcode)

*(※上記アセットデータは本リポジトリのソースコードには含まれず、開発者のローカル環境および外部ストレージでのみ検証用途として利用しています。)*

本プロジェクトの C++ ソースコードは MIT License の下で公開されています。
