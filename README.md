# ToyboxSpikes (SpikesKit)

## Overview

ToyboxSpikes は、自作ゲームエンジン（LREngine）へ組み込むための基盤技術を先行検証（Spike）する目的で開発している、C++20 / DirectX 11 ベースの検証用プロジェクトです。

RHI（Render Hardware Interface）の抽象化をはじめ、以下のような低レイヤ基盤を検証・実装しています。

- **マルチスレッド基盤**：`std::atomic` を用いたロックフリーな描画コマンド構築用ジョブシステム（`RenderJobSystem`）と、非同期アセットロード用の汎用 `JobSystem`
- **メモリ管理**：`RenderPass` 等の構造管理は OOP で拡張性を保ちつつ、描画コマンド構築フェーズでは 32MB の `LinearAllocator` から独立バッファを切り出し、ロック競合と動的確保を排除
- **アセット・イベント基盤**：ステートレスなインポーターによる非同期ロードパイプライン、世代管理コンテナ（`SlotMap`）、関数ポインタベースのイベント通知（`EventDispatcher`）

将来的な DirectX 12 / Vulkan への拡張を前提に、保守性と実行速度のバランスを確認するための構成となっています。

## Demonstrations

本リポジトリには、検証目的ごとに以下の 3 つのデモアプリケーションが含まれています。

- **OpaqueRenderingDemo**
  - Sponza Base Scene を用いた不透明描画の検証デモです。
  - Tracy Profiler を組み込み、非同期アセットロードおよび `RenderJobSystem` による並列コマンド構築の挙動を計測・確認できます。
- **TransparentRenderingDemo**
  - アルファテスト（Mask）および深度書き込み無効（Depth Write Zero）の半透明（Alpha Blending）描画パスの検証デモです。
  - ※厳密な半透明 Z ソートは本検証のスコープ外のため未実装です。
- **DebugRendererDemo**
  - Box / Sphere / Capsule / Cone / Arrow 等のデバッグプリミティブを線分（LineList）として 1 つの動的頂点バッファへまとめ、1 回のドローコールで描画する検証デモです。

## Controls

起動後、以下の操作でカメラを移動できます。

- **カメラ移動**：右クリックを押しながら `W` / `A` / `S` / `D` キー（`Left Shift` 押下で移動加速）
- **視点回転**：右クリックを押しながらマウス移動

## Core Architecture

### 1. RenderJobSystem（並列コマンド構築）
汎用スレッドプールとは分離した、描画コマンド構築専用のロックフリー基盤です。描画ループ内の `std::mutex` による待ちを排除し、`std::atomic` でスレッド間同期を行っています。

- **SPMC Ring Buffer**：`compare_exchange_weak`（CAS）とビットマスク（`& 511`）を用いた固定長リングバッファによるジョブキュー
- **CPU 間同期**：C++20 の `std::latch` を用いたパス単位の完了待機
- **コマンド構築のメモリ配置**：描画リクエストをフラットな `RenderCommand` 構造体へ変換し、事前確保した 32MB の `LinearAllocator` から各ジョブ用の独立バッファ（コマンド用・一時データ用）を切り出して書き込むことで、書き込み時のロック競合とコマンド構築フェーズでの動的確保を排除

### 2. RHI Abstraction
グラフィックス API に依存しないフロントエンド（コマンド構築層）と、DirectX 11 API を呼び出すバックエンド（`SpikesKit/DirectX/DX11/`）を DLL 境界で分離しています。

- **PSO エミュレーション**：DirectX 11 上でありながら、DirectX 12 の PipelineStateObject（PSO）や ShaderBindingsLayout に近いインターフェースで状態管理を統一
- **DLL 境界の寿命管理**：EXE と DLL 間で異なる C ランタイム（CRT）によるヒープ破損を防ぐため、DLL 側で生成したオブジェクトは内部の `Release()`（`delete this;`）経由で破棄

### 3. Asynchronous Asset Pipeline
メインスレッドを止めずにアセットを読み込む非同期ロード基盤と、ハンドルベースのリソース管理を実装しています。

- **ステートレスなインポーター（Texture / glTF 2.0）**：インポータークラスに状態を持たせない設計とし、ワーカースレッドからの同時デコードをロックなしで並列実行
- **クリティカルセクションの最小化**：ファイル I/O やデコードはスレッドローカルで完結させ、`SlotMap` への登録時のみ `std::shared_mutex` でロック
- **型安全な世代管理ハンドル**：64bit ハンドル（`TypedHandle<T>`：上位 32bit に世代、下位 32bit にインデックス）と世代値の奇偶判定（奇数：有効／偶数：削除済み）を `SlotMap` と組み合わせ、追加フラグなしで O(1) アクセスと不正参照防止を両立

## Build Instructions

### Requirements
- **OS**：Windows 10 / 11 (x64)
- **IDE / Compiler**：Visual Studio 2022 または 2026（MSVC, C++20 サポート必須）
- **Build Tool**：CMake 3.xx+（Visual Studio 2026 を使用する場合は CMake 4.2+ が必須）
- **SDK**：Vulkan SDK
  - ※現在の実装は DirectX 11 のみですが、将来のマルチ API 対応を見据えてビルド構成に組み込んでいるため導入が必要です。

### Setup & Build
1. リポジトリ直下の `SetupVS2022.bat` または `SetupVS2026.bat` を、使用する Visual Studio のバージョンに合わせて実行します。
2. `build-msvc` ディレクトリと Visual Studio のソリューションファイル（`.sln`）が生成されます。
3. 生成されたソリューションを開き、Visual Studio 上からビルドしてください。

### Assets Setup
リポジトリ容量削減のため、3D モデルやテクスチャデータ（Sponza 等）は Git 管理から除外しています。デモを実行する際は、初回のみ以下の手順でアセットを配置してください。

1. 本リポジトリの Releases ページからアセットパックのアーカイブ（`.zip`）をダウンロードします。
2. 展開したデータをプロジェクト内の `Asset/Mesh/` ディレクトリ配下に配置します。
3. ビルドしたデモアプリケーションを実行します。

## Credits & Licenses

本プロジェクトのデモ実行用として、以下のサードパーティ製 3D モデルデータを使用しています。アセットの著作権は、各クリエイターおよび権利者に帰属します。

- **Alpha Blend Mode Test**
  - Source: glTF-Sample-Models
  - License: CC BY 4.0 International (SPDX: CC-BY-4.0)
- **Sponza Base Scene**
  - Authors: Frank Meinl and Anton Kaplanyan (Commissioned by Frank Meinl, sponsored by Anton Kaplanyan)
  - Sponza Addon Package Crew: Katica Putica, Cristiano Siqueira, Timothy Heath, Justin Prazen, Sebastian Herholz, Bruce Cherniak, Anton Kaplanyan
  - Additional credit for reference photos: Katica Putica, Princino.photo (www.princinophoto.com), Dubrovnik, Croatia
  - Source: Intel Sample Library
  - License: Creative Commons Attribution 4.0 International (CC BY 4.0)

※上記アセットデータは本リポジトリのソースコードには含まれず、検証用途として Releases のアセットパックおよびローカル環境でのみ使用しています。

本プロジェクトの C++ ソースコードは MIT License の下で公開されています。