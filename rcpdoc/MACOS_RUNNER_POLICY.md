# macOS GitHub Actions runner の運用方針

## 目的

macOS runner イメージの更新に伴う予期しない CI・リリースビルドの失敗を避け、確認できた環境を当面維持する。変更範囲は macOS runner を使う job に限定する。

## 対象と現在の方針

作業ブランチ `feature/rcp2-parser-skeleton` では、macOS runner を使用する次の job に `macos-15` を指定する。

- `.github/workflows/build.yml` の `macos` job
- `.github/workflows/release.yml` の `pkg-macos` job

`.github/workflows/pages.yml` と `.github/workflows/web.yml` は Ubuntu runner のみを使うため、今回の macOS runner 固定の対象外とする。Windows・Linux の job も変更しない。

`macos-latest` が誤った指定という意味ではない。これは GitHub が提供する最新の macOS runner イメージを指すため、将来のイメージ更新の影響を受ける可能性がある。`macos-15` は macOS 15 の runner を明示し、環境を固定しやすくするための暫定的な選択である。

この変更だけを根拠に、過去の失敗原因が runner イメージの更新だったと断定しない。

## 変更範囲と確認ルール

- 変更対象は macOS job の `runs-on` 指定に限定する。
- ビルド手順、依存関係、Windows・Linux・Ubuntu job は、別の根拠がない限り変更しない。
- 変更後は GitHub Actions の CI 結果を確認する。
- CI の成功と継続リリースの成功は区別する。リリース workflow は実際の実行結果、生成物、公開処理を別途確認する。
- 実行結果を確認するまでは、「設定を変更した」と「ビルドが成功した」を区別して記録する。
- RCP2 の解析・実装状況と、この runner 運用方針の検証状況を混同しない。

## 見直し時期

以下はプロジェクト内の目安であり、GitHub が指定する期限ではない。

1. **2026年11月中旬**：CI の実行履歴を確認し、macOS job の成功状況と失敗の再発有無を確認する。
2. **2027年1月ごろ**：GitHub-hosted runner の提供状況、使用する SDK・ツールチェーン・依存関係の対応状況、CI とリリースビルドの実績を確認する。
3. その後も、runner イメージや主要依存関係に大きな更新があった際に見直す。

日付が来たことだけを理由に `macos-latest` へ戻さない。状況に変化がなければ `macos-15` を維持してよい。

## `macos-latest` に戻す判断基準

次の条件を確認できた場合に、runner 指定の変更を独立した小さな変更として試験する。

- GitHub の runner イメージ情報を確認し、`macos-latest` が指す環境を把握している。
- 使用中の SDK・ツールチェーン・依存関係がその環境に対応している。
- `build.yml` の macOS CI が成功する。
- `release.yml` の macOS ビルド・テスト・パッケージ作成も成功する。

失敗した場合はログを確認し、原因が分かるまでは安定している `macos-15` に戻す。

## 確認チェックリスト

- [ ] `build.yml` の macOS job が成功している
- [ ] 失敗時はログと失敗箇所を確認した
- [ ] `release.yml` の macOS ビルド・テスト・パッケージ作成を別途確認した
- [ ] runner イメージと SDK・依存関係の対応状況を確認した
- [ ] runner を再変更した場合、変更と検証結果を記録した

## 参考情報

- [GitHub Actions: Choosing GitHub-hosted runners](https://docs.github.com/en/actions/how-tos/write-workflows/choose-where-workflows-run/choose-the-runner-for-a-job)
- [GitHub-hosted runners: runner images](https://github.com/actions/runner-images)

---

この文書は運用方針と見直し目安を記録するもの。CI・リリースビルドの成功状況は、対象ブランチの GitHub Actions の実行結果を確認して更新する。
