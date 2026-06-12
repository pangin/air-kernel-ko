# 버전 체계 (VERSIONING)

EZ-FLASH Air 한글 커널의 버전 규칙 문서입니다.

## 형식

```
v<upstream>-ko.<build>
```

| 구성요소 | 의미 |
|---|---|
| `<upstream>` | 이 트리가 추종하는 **공식 EZ-FLASH Air 커널 버전**. 현재 `1.04` — 커널 도움말 화면의 `K:1.04` 표기(`source/helpwindow.c`의 `Show_ver`) 기준 |
| `<build>` | **한글화 릴리스 카운터**. `1`부터 시작하며, `<upstream>`이 올라가면 `1`로 리셋 |

- 정식 릴리스 버전 문자열: `1.04-ko.1`, `1.04-ko.2`, …
- 개발(dev) 빌드: `1.04-ko.dev+g<shortsha>` (예: `1.04-ko.dev+gabc1234`)

## 릴리스 절차

릴리스는 **git 태그 push**로 만들어집니다. 태그를 올리면
`.github/workflows/release.yml`이 테스트 → 빌드 → 패키징 후
GitHub Release에 `ezairkernel-v<...>.bin`, `.gba`, `SHA256SUMS.txt`를 첨부합니다.

```sh
git tag v1.04-ko.1
git push origin v1.04-ko.1
```

## 소스 내 매크로 (source/version.h)

`tools/gen_version.py`가 빌드 전에 `source/version.h`를 갱신합니다
(CI의 build 잡이 `make` 전에 실행). 저장소에 커밋된 기본값은 dev 플레이스홀더입니다.

| 매크로 | 용도 | 예 |
|---|---|---|
| `KERNEL_KO_VERSION` | 전체 버전 문자열 | `"1.04-ko.2"`, `"1.04-ko.dev+gabc1234"` |
| `KERNEL_KO_DISPLAY` | 도움말 화면 표시용 압축형 (**10자 이하**, 80px 슬롯) | `"1.04ko2"`, `"1.04ko-dev"` |

버전 결정 우선순위 (`tools/gen_version.py`):

1. `$GITHUB_REF_NAME`이 `^v(\d+\.\d+)-ko\.(\d+)$`에 일치 → 해당 릴리스 버전 (태그 빌드)
2. `git describe --tags --match 'v*-ko.*' --always` 파싱
   - 정확히 태그 위 → 릴리스 버전
   - `v1.04-ko.2-5-gabc1234` 형태 → dev 빌드 (`1.04-ko.dev+gabc1234`)
   - 태그가 전혀 없으면 sha만 → 기본 upstream(`1.04`)의 dev 빌드
3. 아무 정보도 없으면 → `1.04-ko.dev` 플레이스홀더

## 예시

| git 태그 / 상태 | `KERNEL_KO_VERSION` | `KERNEL_KO_DISPLAY` | 의미 |
|---|---|---|---|
| `v1.04-ko.1` | `1.04-ko.1` | `1.04ko1` | 업스트림 1.04 기반 첫 한글화 릴리스 |
| `v1.04-ko.2` | `1.04-ko.2` | `1.04ko2` | 같은 업스트림의 두 번째 한글화 릴리스 |
| 태그 없음 (개발 커밋) | `1.04-ko.dev+gabc1234` | `1.04ko-dev` | CI/로컬 개발 빌드 |
| `v1.05-ko.1` | `1.05-ko.1` | `1.05ko1` | 업스트림 1.05 리베이스 후 첫 릴리스 (**카운터 리셋**) |

## 근거 (rationale)

- **원본 버전 추종 + 한글화 빌드 번호 분리**: 사용자는 자신의 커널이 어떤
  공식 버전 기반인지(`1.04`)와 한글화 패치가 몇 번째 개정판인지(`ko.N`)를
  한 눈에 구분할 수 있습니다.
- **정렬 가능**: `v1.04-ko.1 < v1.04-ko.2 < v1.05-ko.1` 순서가 문자열/시맨틱
  정렬 모두에서 자연스럽게 유지됩니다.
- **GitHub Release 태그와 1:1**: 태그 이름이 곧 버전이며, release.yml이
  태그 push만으로 결정적으로 같은 산출물을 만들어 냅니다.
- **dev 빌드 식별**: 릴리스가 아닌 바이너리에는 항상 `+g<shortsha>`가 붙어
  버그 리포트 시 커밋을 특정할 수 있습니다.
- **화면 제약 반영**: 도움말 화면 버전 슬롯은 약 80px(6px ASCII 기준 약
  13자 중 여유분 포함 10자 제한)이므로 `KERNEL_KO_DISPLAY`는 10자를 넘지
  않도록 `gen_version.py`가 강제합니다.
