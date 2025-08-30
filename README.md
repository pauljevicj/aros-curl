# Aros Curl

Ovo je program napisan u C jeziku koji šalje HTTP GET zahtev ka zadanom URL-u i ispisuje odgovor u konzoli.

## Funkcionalnosti
- Parsira URL u obliku: `http://host[:port]/path`
- Podržava podrazumevani port 80 ako nije naveden
- Usmerava GET zahtev ka hostu
- Ispisuje ceo odgovor sa servera na konzolu

---

## Pokretanje

### 1. Preuzimanje sa udaljenog Git repozitorijuma

```bash
git clone https://github.com/pauljevicj/aros_curl.git
```

### 2. Nalazenje programa na sistemu

Pozicioniranjem u folder u koji je preuzet repozitorijum, mozemo izvrisiti komandu:

```bash
cd aros_curl
```

### 3. Kompajliranje

Putem GCC komande, mozemo da kompajliramo kod

```bash
gcc -o aros_curl aros_curl.c
```

-o aros_curl određuje ime izvršnog fajla.

### 4. Izvrsavanje

```bash
./aros_curl http://example.com
```
