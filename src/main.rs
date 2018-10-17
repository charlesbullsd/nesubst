use std::io::{self, Read, Write};
use std::env;

fn get_env(param: &String) -> String {
    match env::var(param) {
        Ok(val) => val,
        _ => String::new(),
    }
}

fn eval_expr(expr: &String) -> String {
    let mut i: usize = 0;
    for byte in expr.bytes() {
        match byte as char {
            '-' => {
                let param: String = expr.chars().take(i).collect();
                return match env::var(param) {
                    Ok(val) => val,
                    _ => {
                        let def: String = expr.chars().skip(i+1).collect();
                        match def.chars().nth(0).unwrap() {
                            '$' => get_env(&(def.chars().skip(1).collect())),
                            _ => def,
                        }
                    },
                }
            },
            _ => i += 1,
        }
    }

    get_env(expr)
}

fn main() -> io::Result<()> {
    let mut buf = String::new();

    let mut out = io::stdout();

    let mut is_esc = false;
    let mut is_var = false;
    let mut is_exp = false;
    for byte in io::stdin().bytes() {
        let ch = byte? as char;

        if is_esc {
            is_esc = false;
            match ch {
                '$' => {
                    write!(out, "$")?;
                    continue;
                },

                _ => write!(out, "\\")?,
            }
        } else if is_exp {
            match ch {
                '}' => {
                    write!(out, "{}", eval_expr(&buf))?;
                    buf = String::new();
                    is_exp = false;
                    is_var = false;
                    continue;
                },

                _ => buf.push(ch),
            }
        } else if is_var {
            match ch {
                '$' => if buf.len() > 0 {
                    write!(out, "{}", get_env(&buf))?;
                    buf = String::new();
                } else {
                    is_var = false;
                    write!(out, "$");
                },

                '{' => is_exp = true,

                '0'...'9' | 'A'...'Z' | 'a'...'z' | '_' => if buf.len() > 0 || !('0' <= ch && ch <= '9') {
                    buf.push(ch);
                } else {
                    is_var = false;
                    write!(out, "$");
                },

                _ => {
                    is_var = false;

                    if buf.len() > 0 {
                        write!(out, "{}", get_env(&buf))?;
                        buf = String::new();
                    } else {
                        write!(out, "$")?;
                    }
                },
            }
        }

        if !is_var {
            match ch {
                '$' => is_var = true,

                '\\' => is_esc = true,

                _ => {
                    out.write(&[ch as u8])?;
                },
            }
        }
    }

    Ok(())
}
