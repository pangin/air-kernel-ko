# EZ-FLASH Air Kernel KO — 한국어 커널

## English Summary

Korean localization of the official EZ-FLASH Air kernel (Apache-2.0).
Adds a three-language UI (English / 中文 / 한국어) on top of a UTF-8 text
renderer, Hangul SD filenames (FatFs CP949 + UTF-8 LFN), an extensible
font-block renderer so new languages can be slotted in, and kernel slimming
(the FPGA bitstream is deduplicated against `image.bin` at `0x08195000`,
gated by a CRC32 check). Build with devkitARM r53+ and Python 3:
run `./build.sh` on macOS/Linux to produce `ezairkernel.bin`, or use the
original Windows `.bat` + `Link_kernel_image.exe` flow. The kernel `.gba`
must stay under the 1 MB (`0x100000`) hard limit; `build.sh` enforces it.

---

## 소개

이 프로젝트는 공식 **EZ-FLASH Air 커널**(Apache-2.0)을 기반으로 한
**한국어 커널**입니다. 원본 대비 다음이 추가/변경되었습니다.

| 항목 | 내용 |
|---|---|
| 3개 언어 UI | English / 中文 / 한국어 — 문자열 테이블과 렌더러가 모두 UTF-8 기반으로 동작 |
| 한글 SD 파일명 | FatFs에 CP949 코드페이지 + UTF-8 LFN(긴 파일 이름) 지원을 활성화하여 한글 파일명이 깨지지 않음 |
| 확장형 폰트 블록 렌더러 | `source/draw.c`의 `DrawHZText12()`가 UTF-8을 디코딩한 뒤 폰트 블록(한글 dense 테이블, CJK sparse 인덱스)에서 글리프를 찾음 — 새 언어는 폰트 블록 하나를 추가하는 것만으로 끼워 넣을 수 있음 (아래 [언어 추가 가이드](#언어-추가-가이드-기여) 참고) |
| 커널 슬리밍 | FPGA 비트스트림(`deair_FW3.bin`, 464,544바이트)이 `image.bin` 내부(`0x08195000`)에 동일하게 존재함을 확인(CRC32 `0x0BE63A59`)하고, 커널에 중복 내장하지 않고 해당 주소를 참조 — 절약한 공간을 한글 폰트에 사용 |

## 빌드 방법

### 요구 사항

| 도구 | 버전 | 용도 |
|---|---|---|
| devkitARM | r53 이상 | GBA용 크로스 컴파일러 (libgba 포함 설치) |
| libgba | devkitPro 배포판 | GBA 런타임 라이브러리 |
| Python 3 | 3.8 이상 | 커널/이미지 링크 스크립트 (`tools/link_kernel.py`) |

### macOS / Linux

```sh
cd air-kernel-ko
./build.sh
```

`build.sh`는 다음을 수행합니다.

1. `make` — 커널을 컴파일하여 `air-kernel-ko.gba`를 생성
   (Makefile이 디렉터리 이름을 타깃명으로 사용)
2. **1 MB 예산 검사** — 커널 `.gba`는 `0x100000`(1,048,576바이트)
   **하드 리밋**을 절대 넘을 수 없습니다. `image.bin`이 `0x08100000`에
   링크되기 때문이며, 초과 시 빌드가 실패합니다.
3. `tools/link_kernel.py` — `air-kernel-ko.gba`(1 MB로 패딩)와
   `image.bin`(1,074,848바이트 = `0x1066A0`)을 이어 붙여 최종 업데이트
   파일 `ezairkernel.bin`(**2,123,424바이트** = `0x100000 + 0x1066A0`)을
   생성

### Windows

원본 커널과 동일한 흐름을 그대로 사용할 수 있습니다.

1. `build r53-32.bat` 실행 → `air-kernel-ko.gba` 생성
2. `Link_kernel_image.exe` 실행 → `.gba` + `image.bin` → `ezairkernel.bin`
   (exe가 기대하는 파일명이 다르면 `.gba`를 그 이름으로 변경)

> `Link_kernel_image.exe`와 `tools/link_kernel.py`는 동일한 결과물을
> 만듭니다. 어느 쪽을 사용해도 `ezairkernel.bin`은 2,123,424바이트가
> 되어야 합니다.

## 테스트

devkitARM 없이 호스트(맥/리눅스)에서 도는 테스트가 포함되어 있습니다.

```sh
bash tests/run_all.sh        # 전체: 폰트 헤더 → 문자열 예산 → 렌더러 → 패키징
make -C tests/host test      # 렌더러 단위 테스트만 (ASan/UBSan, 실제 draw.c 컴파일)
```

| 테스트 | 내용 |
|---|---|
| `tests/check_fonts.py` | 폰트 헤더 구조(11,172×24B, 인덱스 정렬, 패킹 규약) |
| `tests/check_lang.py` | lang.c UTF-8 무결성, 3개 로더 일치, 문자열 픽셀 폭 예산 |
| `tests/host/` | `draw.c`를 통째로 컴파일해 가/힣/中 픽셀 단위 검증 + UTF-8 엣지 케이스 |
| `scripts/selftest_link_kernel.sh` | 패키징(1MB 패딩 + image.bin 연접) 검증 |

> **에뮬레이터 테스트 주의:** 커널 단독 `.gba`는 image.bin이 없어 부팅
> 시 FPGA 펌웨어 CRC 오류 화면이 뜹니다(B로 통과 가능, 정상 동작).
> mGBA 등으로 테스트할 때는 합쳐진 `ezairkernel.bin`을 (필요시 `.gba`로
> 이름을 바꿔) 사용하세요 — 첫 1MB가 커널, `0x08100000`부터 image.bin이
> 실기와 동일하게 배치됩니다.

## 폰트 생성

한글/한자 폰트는 소스에 직접 그려 넣는 것이 아니라 **생성된 헤더
파일**입니다.

| 파일 | 내용 |
|---|---|
| `source/font_kor.h` | 한글 음절 전체(U+AC00–U+D7A3, 11,172자) — dense 테이블 `KOR_GLYPHS` |
| `source/font_cjk.h` | 중국어 UI에 필요한 한자 — 정렬된 코드포인트 인덱스 `CJK_INDEX` + 병렬 글리프 테이블 `CJK_GLYPHS` |

이 두 파일은 별도 저장소
**[ezflash-kernel-font-generator](../ezflash-kernel-font-generator)**
에서 생성합니다. 글리프 패킹 포맷 명세와 재생성 방법은 해당 저장소의
README를 참고하세요. 폰트를 바꾸려면 거기서 헤더를 다시 생성하여
`source/`에 복사하면 됩니다.

## 설치 / 사용법

### 커널 설치

1. 빌드 결과물(또는 릴리스의) `ezairkernel.bin`을 SD(TF) 카드 **루트**에
   복사합니다.
2. EZ-FLASH Air의 표준 커널 업데이트 절차에 따라 본체를 켜고 화면의
   안내에 따라 업데이트를 진행합니다.
3. 업데이트가 끝나면 `ezairkernel.bin`은 삭제해도 됩니다.

> **주의:** 커널 업데이트 전에 **NOR에 기록된 게임을 백업**해 두는 것을
> 권장합니다. 업데이트 과정에서 NOR 영역이 영향을 받을 수 있습니다.

### SD 카드 폴더 구성

| 경로 | 용도 |
|---|---|
| `/SAVER` | 게임 세이브 파일(.sav) |
| `/CHEAT/Eng` | 영어 치트 라이브러리 |
| `/CHEAT/Chn` | 중국어 치트 라이브러리 |
| `/CHEAT/Kor` | 한국어 치트 라이브러리 |
| `/IMGS` | 게임 썸네일 이미지 |

> **치트 파일 인코딩:** 이 커널의 렌더러는 UTF-8을 사용합니다. `.cht`
> 파일의 치트 이름도 **UTF-8**이어야 화면에 제대로 표시됩니다. 구형
> GB2312로 인코딩된 중국어 치트 라이브러리는 이름이 깨져 보일 수
> 있으니 UTF-8로 변환해 사용하세요.

### 언어 변경

`SETTINGS > LANGUAGE`에서 **English / 中文 / 한국어** 3개 중 선택할 수
있습니다. 선택값은 설정에 저장되어 재부팅 후에도 유지됩니다.

### 단축키

| 키 조합 | 동작 |
|---|---|
| `L + R + START` | 소프트 리셋 (게임 중 커널 메뉴로 복귀) |
| `L + R + SELECT` | 치트 켜기/끄기 |
| `SELECT` | 파일 목록에서 썸네일 표시 토글 |
| `L + START` | 선택한 파일 삭제 |
| `L + A` | 멀티부트(콜드 부트) 실행 |

## 언어 추가 가이드 (기여)

이 커널은 새 언어를 비교적 작은 변경으로 추가할 수 있도록 설계되어
있습니다. 체크리스트:

1. **폰트 블록 생성** —
   [ezflash-kernel-font-generator](../ezflash-kernel-font-generator)
   저장소에서 해당 문자 집합의 글리프 헤더를 생성합니다(24바이트/글리프
   패킹, 산재 문자 집합은 정렬된 `u16` 인덱스 포함). 생성된 헤더를
   `source/`에 추가하고, `source/draw.c`의 폰트 블록 목록(FontBlock
   엔트리)에 코드포인트 범위/인덱스와 글리프 테이블을 등록합니다.
2. **문자열 테이블 추가** — `source/lang.c`에 `xx_*` 문자열 테이블(UTF-8)과
   `LoadXxx()` 로더 함수를 추가하고, `source/lang.h`에
   `extern void LoadXxx(void);` 프로토타입과 `extern char * gl_xx_lang;`을
   선언합니다. 세 로더 모두 언어 라벨(`"ENG"`, `"中文"`, `"한국어"`, …)을
   항상 동일한 상수로 설정해야 언어 선택 화면에 모든 언어가 표시됩니다.
3. **언어 선택 연결** — `source/ezkernelnew.c`의 `CheckLanguage()`에 새
   매직 값 분기를 추가하고(기존: `0xE1E1`=English, `0xE2E2`=中文,
   `0xE3E3`=한국어; 그 외 → 기본값 `0xE1E1`),
   `source/setwindow.c`의 LANGUAGE 선택기 항목과 `save_setw_info`에
   기록되는 매직 값을 확장합니다.
4. **(선택) 치트 경로** — 언어별 치트 라이브러리를 쓰려면
   `source/showcht.c`에 `/CHEAT/<Lang>` 경로 분기를 추가합니다.

## 버전 / 릴리스

버전 체계는 `v<업스트림>-ko.<빌드>` 형식입니다(예: `v1.04-ko.1`).
자세한 규칙과 릴리스 절차는 [VERSIONING.md](VERSIONING.md)를 참고하세요.
태그를 푸시하면 CI가 테스트→빌드 후 GitHub Release에 `ezairkernel.bin`을
자동 첨부합니다.

## 라이선스

| 구성 요소 | 라이선스 |
|---|---|
| EZ-FLASH Air 커널(업스트림) 및 이 프로젝트의 수정 사항 | [Apache-2.0](LICENSE) |
| Galmuri 폰트 — `source/font_kor.h`에 내장된 한글 글리프 데이터는 Galmuri의 파생물 | SIL OFL-1.1 |

상세한 출처 표기는 [NOTICE](NOTICE)를 참고하세요.
